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
const float EPSILON = 0.001f;

//Mandelbulb variables
const float power = 8.0f;
const int iter = 8;
const float bailOut = 2.0f;

vec4 unionSDF(vec4 distA, vec4 distB) {
	if (distA.x < distB.x)
		return distA;
	else
		return distB;
}

float sphereSDF(vec3 samplePoint, vec3 o, float r) {
    return length(samplePoint - o) - r;
}

vec4   Mandelbulb(vec3 pos) {
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	int i;
	for (i = 0; i < iter; ++i) {
		r = length(z);
		if (r > bailOut) break;
		float theta = acos(z.z / r) + uGlobalTime / 2;
		float phi = atan(z.y, z.x) + uGlobalTime / 2;
        float rp = pow(r, power - 1.0);
		dr = rp * power * dr + 1.0;
		float zr = rp * r;
        theta = theta * power;
		phi = phi * power;
		float sinTheta = sin(theta);
		z = zr * vec3(sinTheta * cos(phi), sin(phi) * sinTheta, cos(theta));
		z += pos;
	}
	return vec4(0.25 * log(r) * r / dr, i / 10.0f, 0.4, 0.6);
}

//return vec4(dist, vec3(color))
vec4 SceneSDF(vec3 p) {
	return unionSDF(Mandelbulb(p), vec4(sphereSDF(p, vec3(2 * sin(uGlobalTime), 2 * cos(uGlobalTime), 3 * cos(uGlobalTime)), 0.2f), 1.0f, 1.0f, 0.5f));
}

vec4 ShadowSceneSDF(vec3 p) {
	return Mandelbulb(p);
}

vec3 CalcNormal(vec3 p) {
    return normalize(vec3(
        SceneSDF(vec3(p.x + EPSILON, p.y, p.z)).x - SceneSDF(vec3(p.x - EPSILON, p.y, p.z)).x,
        SceneSDF(vec3(p.x, p.y + EPSILON, p.z)).x - SceneSDF(vec3(p.x, p.y - EPSILON, p.z)).x,
        SceneSDF(vec3(p.x, p.y, p.z + EPSILON)).x - SceneSDF(vec3(p.x, p.y, p.z - EPSILON)).x
    ));
}

#define OCCLUSION_ITERS 5
#define OCCLUSION_STRENGTH 8.0
#define OCCLUSION_GRANULARITY 1.

float   CalcAO( in vec3 hit, in vec3 normal ) {
    float k = 1.0;
    float d = 0.0;
    float occ = 0.0;
    for(int i = 0; i < OCCLUSION_ITERS; i++){
        d = SceneSDF(hit + normal * k * OCCLUSION_GRANULARITY).x;
        occ += 1.0 / pow(3.0, k) * ((k - 1.0) * OCCLUSION_GRANULARITY - d);
        k += 1.0;
    }
    return 1.0 - clamp(occ * OCCLUSION_STRENGTH, 0.0, 1.0);
}

vec4	RayMarche(float start, float end, vec3 ray, vec3 origin)
{
	float depth = start;
	vec4 dist;
	for (int i = 0; i < MAX_MARCHING_STEPS; i++)
	{
		dist = SceneSDF(origin + depth * ray);
		if (dist.x < EPSILON)
			return vec4(depth, dist.yzw);
		depth += dist.x;
		if (depth >= end)
			return vec4(end, dist.yzw);
	}
	return vec4(end, dist.yzw);
}

bool CalcShadow(float start, float end, vec3 ray, vec3 origin)
{
	float res = 1.0f;
	for (float t = start; t < end;)
	{
		float h = ShadowSceneSDF(origin + t * ray).x;
		if (h < EPSILON)
			return true;
		t += h;
	}
	return false;
}

vec3 phongContribForLight(vec3 k_d, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity, vec3 N, vec3 L) {
    L = normalize(L);

    float dotLN = dot(L, N);

    if (dotLN < EPSILON)
        return vec3(0.0, 0.0, 0.0);

	float ao = CalcAO(p, N);
    return lightIntensity * k_d * dotLN * ao;
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;

    vec3 lightPos = vec3(2 * sin(uGlobalTime), 2 * cos(uGlobalTime), 3 * cos(uGlobalTime));
    vec3 lightIntensity = vec3(1.0, 1.0, 1.0);

    vec3 N = CalcNormal(p);
	vec3 L = lightPos - p;

    color += phongContribForLight(k_d, p, eye, lightPos, lightIntensity, N, L);

	if (CalcShadow(0.0f, length(L), normalize(L), p + N * 0.01))
		color = max(color - 0.2f, vec3(0.0f, 0.0f, 0.0f));
    return color;
}


//calc NDC from gl_fragCoord : https://www.khronos.org/opengl/wiki/Compute_eye_space_from_window_space

vec3 CalcRayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
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
	vec3 ray = CalcRayDirection(uFov, uResolution, gl_FragCoord.xy);

	vec4 dist = RayMarche(MIN_DIST, MAX_DIST, ray, uCamPos);

    if (dist.x > MAX_DIST - EPSILON)
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	else
	{
		vec3 p = uCamPos + dist.x * ray;

		vec3 K_d = vec3(dist.y, dist.z, dist.w);
		vec3 K_a = K_d;

		vec3 color = phongIllumination(K_a, K_d, p, uCamPos);

		FragColor = vec4(color, 1.0);
	}
}

