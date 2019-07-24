#version 330 core
out vec4 FragColor;

in mat4 viewMat;
in mat4 mvp;
in mat4 proj;

uniform vec2	uResolution;//Resolution of screen
uniform float	uFov;//Fov in radians
uniform vec3	uCamPos;//Position of camera in world
uniform vec2	uRotation;//Rotation of the camera
uniform vec3	uDir;//Dir of the camera
uniform vec3	uUp;//Dir of the camera
uniform float	uGlobalTime;
uniform vec3	uOrigin;

const int MAX_MARCHING_STEPS = 300;
const float MIN_DIST = 0.0f;
const float MAX_DIST = 100.0f;
const float EPSILON = 0.001f;

float unionSDF(float distA, float distB) {
    return min(distA, distB);
}

float differenceSDF(float distA, float distB) {
    return max(distA, -distB);
}


float cubeSDF(vec3 p, vec3 size) {
    vec3 d = abs(p) - size;
    float insideDistance = min(max(d.x, max(d.y, d.z)), 0.0);
    float outsideDistance = length(max(d, 0.0));
    return insideDistance + outsideDistance;
}

float crossSDF(vec3 p, vec2 size)
{
  float da = cubeSDF(p,vec3(size.y,size.x,size.x));
  float db = cubeSDF(p,vec3(size.x,size.y,size.x));
  float dc = cubeSDF(p,vec3(size.x,size.x,size.y));
  return unionSDF(da, unionSDF(db,dc));
}

float menger(vec3 p)
{
	float speed = cos(uGlobalTime * .1) + 1.0f;
	float d = cubeSDF(p, vec3(speed, speed, speed));
	float s = 1.0;
	vec3 r;
	for(int i = 0; i < 3; i++)
	{
		vec3 a = mod(p * s, 2.0f) - 1.0f;
		s *= 3.0;
		r = 1.0 - 3.0 * abs(a);

		float c = crossSDF(r, vec2(0.5f, 5.0f)) / s;
		d = differenceSDF(d, c);
	}
	return d;
}

float SceneSDF(vec3 p) {
	p -= uOrigin;
	return menger(p);
}

vec3 CalcNormal(vec3 p) {
    return normalize(vec3(
        SceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - SceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        SceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - SceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        SceneSDF(vec3(p.x, p.y, p.z + EPSILON)) - SceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

float	RayMarche(float start, float end, vec3 ray, vec3 origin)
{
	float depth = start;
	float dist;
	for (int i = 0; i < MAX_MARCHING_STEPS; i++)
	{
		dist = SceneSDF(origin + depth * ray);
		if (dist < EPSILON)
			return depth;
		depth += dist;
		if (depth >= end)
			return end;
	}
	return end;
}

vec3 phongContribForLight(vec3 k_d, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity, vec3 N, vec3 L) {
    L = normalize(L);

    float dotLN = dot(L, N);

    if (dotLN < EPSILON)
        return vec3(0.0, 0.0, 0.0);

    return lightIntensity * k_d * dotLN;
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;

    vec3 lightPos = vec3(0, 0, 5);
	lightPos += uOrigin;
    vec3 lightIntensity = vec3(1.0, 1.0, 1.0);

    vec3 N = CalcNormal(p);
	vec3 L = lightPos - p;

    color += phongContribForLight(k_d, p, eye, lightPos, lightIntensity, N, L);

    return color;
}


//calc NDC from gl_fragCoord : https://www.khronos.org/opengl/wiki/Compute_eye_space_from_window_space

vec3 CalcRayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
	float imageAspectRatio = size.x / size.y;
	vec4 viewport = vec4(0, 0, size.x, size.y);
	vec2 ndc = ((2.0 * gl_FragCoord.xy) - (2.0 * viewport.xy)) / (viewport.zw) - 1;
	vec3 ndc3 = vec3(ndc, -1);
	vec3 ray = (inverse(proj) * vec4(ndc3, 1.0f)).xyz;
	ray = normalize((inverse(viewMat) * vec4(ray, 0.0f)).xyz);
	return ray;
}

void main()
{
	vec3 ray = CalcRayDirection(uFov, uResolution, gl_FragCoord.xy);

	float dist = RayMarche(MIN_DIST, MAX_DIST, ray, uCamPos);

    if (dist > MAX_DIST - EPSILON)
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	else
	{
		vec3 p = uCamPos + dist * ray;

		vec3 K_d = vec3(0.4f * length(p - uOrigin), 0.1f, 0.8f);
		vec3 K_a = K_d;

		vec3 color = phongIllumination(K_a, K_d, p, uCamPos);

		FragColor = vec4(color, 1);
	}
}

