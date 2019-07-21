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

#define M_PI 3.1415926535897932384626433832795

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

	const float Power = 8.0;

vec2   mandelbulb(vec3 pos) {
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	float t0 = 1.0f;
	int i;
	for (i = 0; i < 8; ++i) {
		r = length(z);
		if (r > 2.0) break;
		float theta = acos(z.z / r);// + uGlobalTime / 2;
		float phi = atan(z.y, z.x);// + uGlobalTime / 2;
        float rp = pow(r, Power - 1.0);
		dr = rp * Power * dr + 1.0;
		float zr = rp * r;
        theta = theta * Power;
		phi = phi * Power;
		float sinTheta = sin(theta);
		z = zr * vec3(sinTheta * cos(phi), sin(phi) * sinTheta, cos(theta));
		z += pos;
		t0 = min(t0, zr);
	}
	return vec2(0.25 * log(r) * r / dr, i);
}


vec2 sceneSDF(vec3 p) {
	return mandelbulb(p);
}

vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)).x - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)).x,
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)).x - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)).x,
        sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)).x - sceneSDF(vec3(p.x, p.y, p.z - EPSILON)).x
    ));
}

#define OCCLUSION_ITERS 5
#define OCCLUSION_STRENGTH 64.0
#define OCCLUSION_GRANULARITY 1.

float   ambientOcclusion( in vec3 hit, in vec3 normal ) {
    float k = 1.0;
    float d = 0.0;
    float occ = 0.0;
    for(int i = 0; i < OCCLUSION_ITERS; i++){
        d = sceneSDF(hit + normal * k * OCCLUSION_GRANULARITY).x;
        occ += 1.0 / pow(3.0, k) * ((k - 1.0) * OCCLUSION_GRANULARITY - d);
        k += 1.0;
    }
    return 1.0 - clamp(occ * OCCLUSION_STRENGTH, 0.0, 1.0);
}

vec2	rayMarche(float start, float end, vec3 ray, vec3 origin)
{
	float depth = start;
	vec2 dist;
	for (int i = 0; i < MAX_MARCHING_STEPS; i++)
	{
		dist = sceneSDF(origin + depth * ray);
		if (dist.x < EPSILON)
			return vec2(depth, dist.y);
		depth += dist.x;
		if (depth >= end)
			return vec2(end, dist.y);
	}
	return vec2(end, dist.y);
}

bool shadow(float start, float end, vec3 ray, vec3 origin)
{
	float res = 1.0f;
	for (float t = start; t < end;)
	{
		float h = sceneSDF(origin + t * ray).x;
		if (h < EPSILON)
			return true;
		t += h;
	}
	return false;
}


vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = estimateNormal(p);
    vec3 L = normalize(lightPos - p);

    float dotLN = dot(L, N);

    if (dotLN < EPSILON)
        return vec3(0.0, 0.0, 0.0);

	float ao = ambientOcclusion(p, N);
    return lightIntensity * k_d * dotLN;// * ao;
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.3 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;

    vec3 N = estimateNormal(p);
    //vec3 light1Pos = vec3(4 * sin(uGlobalTime), 5 * cos(uGlobalTime), 0.0f);
    vec3 light1Pos = vec3(0, 5, 0.0f);
    vec3 light1Intensity = vec3(0.8, 0.8, 0.8);

    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity);

	if (shadow(0.0f, length(light1Pos - p), normalize(light1Pos - p), p + N * 0.01))
		;//color = max(color - 0.2f, vec3(0.0f, 0.0f, 0.0f));
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

	vec2 dist = rayMarche(MIN_DIST, MAX_DIST, ray, uCamPos);

    if (dist.x > MAX_DIST - EPSILON) {
        // Didn't hit anything
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
		return;
    }

    // The closest point on the surface to the eyepoint along the view ray
    vec3 p = uCamPos + dist.x * ray;

	vec3 K_d = vec3(dist.y / 10, 0.4, 0.6);
    vec3 K_a = K_d;
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;

    //vec3 color = K_d;
    vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p, uCamPos);

    FragColor = vec4(color, 1.0);
}

