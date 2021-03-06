#version 330 core
out vec4 FragColor;

in mat4 invertViewMat;
in mat4 invertProjMat;

uniform vec2		uResolution;//Resolution of screen
uniform float		uFov;//Fov in radians
uniform vec3		uCamPos;//Position of camera in world
uniform vec2		uRotation;//Rotation of the camera
uniform vec3		uDir;//Dir of the camera
uniform vec3		uUp;//Dir of the camera
uniform float		uGlobalTime;
uniform vec3		uOrigin;
uniform sampler2D	uNoise;

const int MAX_MARCHING_STEPS = 300;
const float MIN_DIST = 0.0f;
const float MAX_DIST = 100.0f;
const float EPSILON = 0.001f;
const int maxStep = 100;
const float step = 0.05f;

float noise(vec3 x)
{
	vec3 p = floor(x);
	vec3 f = fract(x);
	f = f *f *(3.0 - 2.0 * f);

	vec2 uv = (p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
	vec2 rg = texture(uNoise, (uv + 0.5) / 256.0, -100.0).yx;
	return mix(rg.x, rg.y, f.z);
}

float cloudNoise(vec3 p, vec3 dir)
{
	vec3 q = p + dir;
	float amplitude = 0.5;
	float granularity = 2.0;
	float f = 0;
	for (int i = 0; i < 4; i++)
	{
		f += amplitude * noise(q);
		q = q * granularity + dir;
		amplitude /= 2.0;
	}
	return f;
}

float sphereSDF(vec3 samplePoint, vec3 o, float r) {
	return length(samplePoint - o) - r;
}

float SceneSDF(vec3 p) {
	p -= uOrigin;
	float noise = cloudNoise(p, vec3(0.0f, 0.25f, 0.125f) * uGlobalTime); 
	float d = -sphereSDF(p, vec3(0), 0.5f);
	return clamp(d + 2.0f * noise, 0.0f, 1.0f);
}

vec4	RayMarche(vec3 ray, vec3 origin)
{
	vec4 sum = vec4(0.0f), col;
	float t = 0.0f, d;
	vec3 pos;
	for (int i = 0; i < maxStep; i++)
	{
		if (sum.a > 0.99f)
			break;
		pos = origin + ray * t;
		d = SceneSDF(pos);

		col = vec4(mix(vec3(1.0f), vec3(0.0f), d), 1.0f);

		col *= d;
		sum += col * (1.0f - sum.a);
		t += step;
	}
	return clamp(sum, 0.0f, 1.0f);
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

vec2 sphereIntersect(vec3 origin, vec3 ray, vec3 o, float r)
{
    vec3 oc = origin - o;
    float b = dot(oc, ray);
    float c = dot(oc, oc) - r * r;
    float h = b * b - c;
    if(h < 0.0) return vec2(-1.0);
    h = sqrt(h);
    return vec2(-b - h, -b + h);
}

void main()
{
	vec3 ray = CalcRayDirection(uFov, uResolution, gl_FragCoord.xy);

	vec2 rayHit = sphereIntersect(uCamPos, ray, vec3(0) + uOrigin, 2.0f);
	vec4 col;
	if (rayHit.x >= 0 || rayHit.y >= 0)
		col = RayMarche(ray, uCamPos + ray * max(rayHit.x, 0.0));
	else
		col = vec4(0.0f);

	FragColor = col;
	return;
}


