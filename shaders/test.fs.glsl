#version 330 core
out vec4 FragColor;

in mat4 invertViewMat;
in mat4 invertProjMat;

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
const float EPSILON = 0.0001f;

struct obj
{
	vec3	rgb;
	float	d;
};

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

float unionSDF(float distA, float distB) {
    return min(distA, distB);
}

float differenceSDF(float distA, float distB) {
    return max(distA, -distB);
}

float sphereSDF(vec3 samplePoint, vec3 o, float r) {
	return length(samplePoint - o) - r;
}

float CylinderSDF(vec3 p, vec3 o, vec3 c, mat3 rot)
{
	p -= o;
	p *= rot;
	return length(p.xz-c.xy)-c.z;
}

float torusSDF(vec3 p, vec3 o, vec2 r, mat3 rot)//r = vec2(big radius, little radius)
{
	p -= o;
	p *= rot;
	vec2 q = vec2(length(p.xz) - r.x, p.y);
	return length(q) - r.y;
}

float BoxSDF(vec3 p, vec3 o, vec3 b)
{
	p -= o;
	vec3 d = abs(p) - b;
	return length(max(d,0.0))
		 + min(max(d.x,max(d.y,d.z)),0.0);
}

float	sphereHoled(vec3 p, vec3 o, float time)
{
	float sphere = mix(sphereSDF(p, o, 1.0f), BoxSDF(p, o, vec3(0.5f)), time);
	float cylinderY = mix(CylinderSDF(p, o, vec3(0.0, 0.0, 0.3), mat3(1.0f)), BoxSDF(p, o, vec3(0.3, 2.0, 0.3)), time);
	float cylinderZ = mix(CylinderSDF(p, o, vec3(0.0, 0.0, 0.3), rotateX(radians(90.0f))), BoxSDF(p, o, vec3(0.3, 0.3, 2.0)), time);
	float cylinderX = mix(CylinderSDF(p, o, vec3(0.0, 0.0, 0.3), rotateZ(radians(90.0f))), BoxSDF(p, o, vec3(2.0, 0.3, 0.3)), time);
	return differenceSDF(differenceSDF(differenceSDF(sphere, cylinderY), cylinderZ), cylinderX);
}

//return vec4(dist, vec3(color))
vec4 SceneSDF(vec3 p) {
	p -= uOrigin;
	float cuGlobalTime = cos(uGlobalTime);
	float suGlobalTime = sin(uGlobalTime);
	float time = cos(uGlobalTime) / 2 + 0.5;
	vec3 spherePos = vec3(max(cuGlobalTime, 0.0) * 1.2, suGlobalTime * 1.2, 0.0f);
	vec3 sphere2Pos = vec3(cos(uGlobalTime + 1) * -1.2, max(sin(uGlobalTime + 1), 0.0) * -1.2, 0.0f);
	vec3 sphere3Pos = vec3(max(suGlobalTime, 0.0) * 1.2, 0.0f, cuGlobalTime * -1.2);
	float sphere = mix(sphereSDF(p, spherePos, .2), BoxSDF(p, spherePos, vec3(.2)), time);
	float sphere2 = mix(sphereSDF(p, sphere2Pos, .2), BoxSDF(p, sphere2Pos, vec3(.2)), time);
	float sphere3 = mix(sphereSDF(p, sphere3Pos, .2), BoxSDF(p, sphere3Pos, vec3(.2)), time);
	return vec4(unionSDF(unionSDF(unionSDF(sphereHoled(p, vec3(0), time), sphere), sphere2), sphere3), vec3(0));
}

vec3 CalcNormal(vec3 p) {
	return normalize(vec3(
				SceneSDF(vec3(p.x + EPSILON, p.y, p.z)).x - SceneSDF(vec3(p.x - EPSILON, p.y, p.z)).x,
				SceneSDF(vec3(p.x, p.y + EPSILON, p.z)).x - SceneSDF(vec3(p.x, p.y - EPSILON, p.z)).x,
				SceneSDF(vec3(p.x, p.y, p.z + EPSILON)).x - SceneSDF(vec3(p.x, p.y, p.z - EPSILON)).x
				));
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
	vec3 ray = (invertProjMat * vec4(ndc3, 1.0f)).xyz;
	ray = normalize((invertViewMat * vec4(ray, 0.0f)).xyz);
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

		vec3 K_d = vec3(.2, .7, .5);
		vec3 K_a = K_d;

		vec3 color = phongIllumination(K_a, K_d, p, uCamPos);

		FragColor = vec4(color, 1.0);
	}
}

