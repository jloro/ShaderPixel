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

const int MAX_MARCHING_STEPS = 300;
const float MIN_DIST = 0.0f;
const float MAX_DIST = 100.0f;
const float EPSILON = 0.0001f;

mat3 rotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);

    return mat3(
        vec3(c, 0, s),
        vec3(0, 1, 0),
        vec3(-s, 0, c)
    );
}

mat3 rotateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);

    return mat3(
        vec3(1, 0, 0),
        vec3(0, c, -s),
        vec3(0, s, c)
    );
}

mat3 rotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);

    return mat3(
        vec3(c, -s, 0),
        vec3(s, c, 0),
        vec3(0, 0, 1)
    );
}
float intersectSDF(float distA, float distB) {
    return max(distA, distB);
}

float unionSDF(float distA, float distB) {
    return min(distA, distB);
}

float differenceSDF(float distA, float distB) {
    return max(distA, -distB);
}

float torusSDF(vec3 p, vec2 r)//r = vec2(big radius, little radius)
{
  vec2 q = vec2(length(p.xz)-r.x,p.y);
  return length(q)-r.y;
}

float cubeSDF(vec3 p, vec3 size) {
    vec3 d = abs(p) - size;
    float insideDistance = min(max(d.x, max(d.y, d.z)), 0.0);
    float outsideDistance = length(max(d, 0.0));
    return insideDistance + outsideDistance;
}

float sdCross(vec3 p, vec2 size)
{
  float da = cubeSDF(rotateX(-uGlobalTime / 2) * p,vec3(size.y,size.x, size.x));
  float db = cubeSDF(rotateY(uGlobalTime / 2) * p,vec3(size.x,size.y,size.x));
  float dc = cubeSDF(rotateZ(-uGlobalTime / 2) * p,vec3(size.x,size.x,size.y));
  //float da = cubeSDF(p,vec3(size.y,size.x, size.x));
  //float db = cubeSDF(p,vec3(size.x,size.y,size.x));
  //float dc = cubeSDF(p,vec3(size.x,size.x,size.y));
  return unionSDF(da, unionSDF(db,dc));
}

float sphereSDF(vec3 samplePoint, vec3 o, float r) {
    return length(samplePoint - o) - r;
}

float cappedCylinderSDF(vec3 p, float h, float r) {
    float inOutRadius = length(p.xy) - r;
    float inOutHeight = abs(p.z) - h/2.0;
    float insideDistance = min(max(inOutRadius, inOutHeight), 0.0);
    float outsideDistance = length(max(vec2(inOutRadius, inOutHeight), 0.0));
    return insideDistance + outsideDistance;
}

float sceneSDF(vec3 p) {
	float d = cubeSDF(p, vec3(1.0f, 1.0f, 1.0f));
	float s = 1.0;
	vec3 r;
	for( int m=0; m<3; m++ )
	{
		vec3 a = mod( p*s, 2.0 )-1.0;
		s *= 3.0;
		r = 1.0 - 3.0*abs(a);

		float c = sdCross(r, vec2(0.5f, 5.0f))/s;
		d = max(d,-c);
	}
	return d;//unionSDF(d, sphereSDF(p, vec3(0.0f, sin(uGlobalTime) * 2,cos(uGlobalTime) * 2), 0.1f));
	//return unionSDF(sphereSDF(p, vec3(0.0f, 0.0f, -5.0f), 1.0f), sphereSDF(p, vec3(0.0f, 0.0f, 0.0f), 1.0f));
}

vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = estimateNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));

    float dotLN = dot(L, N);
    float dotRV = dot(R, V);

    if (dotLN < 0.0)
        return vec3(0.0, 0.0, 0.0);

    if (dotRV < 0.0)
        return lightIntensity * (k_d * dotLN);
    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

float	rayMarche(float start, float end, vec3 ray, vec3 origin)
{
	float depth = start;
	float dist;
	for (int i = 0; i < MAX_MARCHING_STEPS; i++)
	{
		dist = sceneSDF(origin + depth * ray);
		if (dist < EPSILON)
			return depth;
		depth += dist;
		if (depth >= end)
			return end;
	}
	return end;
}
bool	rayMarcheShadow(float start, float end, vec3 ray, vec3 origin)
{
	float dist;
	for (float t = start; t < end;)
	{
		dist = sceneSDF(origin + t * ray);
		if (dist < EPSILON)
			return true;
		t += dist;
	}
	return false;
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;

    vec3 N = estimateNormal(p);
    vec3 light1Pos = vec3(0.0f, 0.0f, 8.0f);
    vec3 light1Intensity = vec3(0.4, 0.4, 0.4);

    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity);

	bool shadow = rayMarcheShadow(EPSILON, length(normalize(light1Pos - p)), normalize(light1Pos - p), p + N * 0.1f);

	if (shadow)
        color = vec3(0.0, 0.0, 0.0);

    return color;
}


//calc NDC from gl_fragCoord : https://www.khronos.org/opengl/wiki/Compute_eye_space_from_window_space

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
	float imageAspectRatio = size.x / size.y;
	vec4 viewport = vec4(0, 0, 800, 400);
	vec2 ndc = ((2.0 * gl_FragCoord.xy) - (2.0 * viewport.xy)) / (viewport.zw) - 1;
	vec3 ndc3 = vec3(ndc, -1);
	vec3 ray = (inverse(proj) * vec4(ndc3, 1.0f)).xyz;
	ray = normalize((inverse(viewMat) * vec4(ray, 0.0f)).xyz);
	return ray;
}

void main()
{
	vec3 ray = rayDirection(uFov, uResolution, gl_FragCoord.xy);

	float dist = rayMarche(MIN_DIST, MAX_DIST, ray, uCamPos);

    if (dist > MAX_DIST - EPSILON) {
        // Didn't hit anything
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
		return;
    }

    // The closest point on the surface to the eyepoint along the view ray
    vec3 p = uCamPos + dist * ray;

    vec3 K_a = vec3(0.2, 0.2, 0.2);
    vec3 K_d = vec3(0.7, 0.2, 0.2);
    vec3 K_s = vec3(0.0, 0.0, 0.0);
    float shininess = 30.0;

    vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p, uCamPos);

    FragColor = vec4(color, 1.0);
}
