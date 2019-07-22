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

# define M_PI 3.1415926535897932384626433832795

vec4 unionSDF(vec4 distA, vec4 distB) {
	if (distA.x < distB.x)
		return distA;
	else
		return distB;
}

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

float cubeSDF(vec3 p, vec3 size) {
    vec3 d = abs(p) - size;
    float insideDistance = min(max(d.x, max(d.y, d.z)), 0.0);
    float outsideDistance = length(max(d, 0.0));
    return insideDistance + outsideDistance;
}

float cappedCylinderSDF(vec3 p, float h, float r) {
    float inOutRadius = length(p.xy) - r;
    float inOutHeight = abs(p.z) - h/2.0;
    float insideDistance = min(max(inOutRadius, inOutHeight), 0.0);
    float outsideDistance = length(max(vec2(inOutRadius, inOutHeight), 0.0));
    return insideDistance + outsideDistance;
}

float sphereSDF(vec3 samplePoint, vec3 o, float r) {
    return length(samplePoint - o) - r;
}

float sdPlane(vec3 p, vec4 n)
{
  // n must be normalized
  return dot(p,n.xyz) + n.w;
}

float sdCappedCone( in vec3 p, in float h, in float r1, in float r2 )
{
    vec2 q = vec2( length(p.xz), p.y );
    
    vec2 k1 = vec2(r2,h);
    vec2 k2 = vec2(r2-r1,2.0*h);
    vec2 ca = vec2(q.x-min(q.x,(q.y<0.0)?r1:r2), abs(q.y)-h);
    vec2 cb = q - k1 + k2*clamp( dot(k1-q,k2)/dot(k2, k2), 0.0, 1.0 );
    float s = (cb.x<0.0 && ca.y<0.0) ? -1.0 : 1.0;
    return s*sqrt( min(dot(ca, ca),dot(cb, cb)) );
}

vec4 Tree(vec3 p, vec3 origin, float rTrunk, float hTrunk, vec3 colorTrunk, float hLeafs, float rLeafs, vec3 colorLeafs)
{
	vec4 leafs = vec4(sdCappedCone((p - vec3(origin.x, hTrunk - 1.0f + origin.y, origin.z)) * rotateX(M_PI),
									hLeafs,
									0.0f,
									rLeafs), colorLeafs);
	vec4 trunk = vec4(cappedCylinderSDF((p - origin) * rotateX(M_PI / 2.0f), hTrunk, rTrunk), colorTrunk);
	return unionSDF(leafs, trunk); 
}

//return vec4(dist, vec3(color))
vec4 SceneSDF(vec3 p) {
	vec4 tree = Tree(mod(p, vec3(2, 0, 0)) - 0.5*vec3(2, 0, 0), vec3(0.0f, 0.0f, -20.0f), 0.5f, 3.0f, vec3(0.23f, 0.13f, 0.01f), 1.0f, 1.0f, vec3(0.27, 0.70f, 0.27f));
	vec4 plane = vec4(sdPlane(p, vec4(0, 1, 0, 1)) , 0.5, 0.50f, 0.57f);
	return unionSDF(plane, tree);
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

	//float ao = CalcAO(p, N);
    return lightIntensity * k_d * dotLN;// * ao;
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;

    vec3 lightPos = vec3(0, 5, 0);
    vec3 lightIntensity = vec3(1.0, 1.0, 1.0);

    vec3 N = CalcNormal(p);
	vec3 L = lightPos - p;

    color += phongContribForLight(k_d, p, eye, lightPos, lightIntensity, N, L);

	//if (CalcShadow(0.0f, length(L), normalize(L), p + N * 0.01))
	//	color = max(color - 0.2f, vec3(0.0f, 0.0f, 0.0f));
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
        FragColor = vec4(0.0, 1.0, 1.0, 1.0);
	else
	{
		vec3 p = uCamPos + dist.x * ray;

		vec3 K_d = vec3(dist.y, dist.z, dist.w);
		vec3 K_a = K_d;

		vec3 color = phongIllumination(K_a, K_d, p, uCamPos);

		FragColor = vec4(color, 1.0);
	}
}

