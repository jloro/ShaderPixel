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

const int MAX_MARCHING_STEPS = 400;
const float MIN_DIST = 0.0f;
const float MAX_DIST = 100.0f;
const float EPSILON = 0.001f;

const float minRadius = 0.25f;
const float minRadius2 = minRadius * minRadius;
const int iter = 8;
const float scale = 2.5f;
const vec4 scalevec = vec4(scale, scale, scale, abs(scale)) / minRadius2;
const float C1 = abs(scale - 1.0f), C2 = pow(abs(scale), float(1.0f - iter));
const float foldingLimit = 1.2f;

vec4   mandelbox(vec3 pos) {
	vec4 p = vec4(pos, 1.0f);
	vec4 p0 = p;
	int i;
	for (i = 0; i < iter; i++)
	{
		p.xyz = clamp(p.xyz, -foldingLimit, foldingLimit) * 2.0f - p.xyz;
		float r2 = dot(p.xyz, p.xyz);
		p *= clamp(max(minRadius2 / r2, minRadius2), 0.0f, 1.0f);
		p = p * scalevec + p0;
	}
	return vec4((length(p.xyz) - C1) / p.w - C2, vec3(.8, .4, .5));
}

//return vec4(dist, vec3(color))
vec4 SceneSDF(vec3 p) {
	return mandelbox((p - uOrigin) * 3.0f) / 3.0f;
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
                          vec3 lightPos, vec3 lightIntensity, vec3 N) {
	vec3 L = lightPos - p;
    L = normalize(L);

    float dotLN = dot(L, N);

    if (dotLN < EPSILON)
        return vec3(0.0, 0.0, 0.0);

    return lightIntensity * k_d * dotLN;
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 p, vec3 eye) {
    const vec3 ambientLight = .2 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;

    vec3 lightPos = uCamPos;
    vec3 lightIntensity = vec3(1.0, 1.0, 1.0);

	vec3 N = CalcNormal(p);
    color += phongContribForLight(k_d, p, eye, lightPos, lightIntensity, N);

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

		float d = length(p - uOrigin);
		vec3 K_d = vec3(dist.y * d, dist.z / d, dist.w * d);
		vec3 K_a = vec3(.3, .3, .3);

		vec3 color = phongIllumination(K_a, K_d, p, uCamPos);

		FragColor = vec4(color, 1.0);
	}
}

