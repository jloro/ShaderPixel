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
uniform sampler2D uNoise;

const int MAX_MARCHING_STEPS = 300;
const float MIN_DIST = 0.0f;
const float MAX_DIST = 100.0f;
const float EPSILON = 0.001f;

float hash( float n ) { return fract(sin(n)*753.5453123); }
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}
float cloudNoise(float scale,in vec3 p, in vec3 dir)
{
	vec3 q = p + dir;
    float f;
	f  = 0.50000*noise( q ); q = q*scale*2.02 + dir;
	//f += 0.25000*noise( q ); q = q*2.03 + dir;
    //f += 0.12500*noise( q ); q = q*2.01 + dir;
    //f += 0.06250*noise( q ); q = q*2.02 + dir;
    //f += 0.03125*noise( q );
    return f;
}

vec4 unionSDF(vec4 distA, vec4 distB) {
	if (distA.x < distB.x)
		return distA;
	else
		return distB;
}

float sphereSDF(vec3 samplePoint, vec3 o, float r) {
    return length(samplePoint - o) - r;
}

//return vec4(dist, vec3(color))
float SceneSDF(vec3 p) {
	p -= uOrigin;
	float noise = cloudNoise(2.0, p, vec3(0.0f, 0.25f, 0.10f) * uGlobalTime); 
	float d = -sphereSDF(p, vec3(0, 0, 0), 1.0f);
	return clamp(d + 2.0f * noise, 0.0f, 1.0f);
}

vec3 CalcNormal(vec3 p) {
    return normalize(vec3(
        SceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - SceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        SceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - SceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        SceneSDF(vec3(p.x, p.y, p.z + EPSILON)) - SceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

const int maxStep = 100;
const float step = 0.05f;
vec4	RayMarche(vec3 ray, vec3 origin)//vec2(distAccum, 1st p)
{
	vec4 sum = vec4(0.0f), col;
	float t = 0.0f;
	float d;
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

vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = CalcNormal(p);
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

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;
    
    vec3 light1Pos = vec3(0, 0, 5);
    vec3 light1Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity);
    
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

	vec4 dist = RayMarche(ray, uCamPos);

	FragColor = dist;
	return;
    /*if (dist.x == 0.0f)
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	else
	{
		vec3 p = uCamPos + dist.y * ray;
		float F = 1.0f / exp(dist.x * .8);
		FragColor = vec4(.2, .2, .3, 1.0f);
		return;
		vec3 K_a = vec3(0.2, 0.2, 0.2);
		vec3 K_d = vec3(0.7, 0.2, 0.2);
		vec3 K_s = vec3(1.0, 1.0, 1.0);
		float shininess = 10.0;

		vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p, uCamPos);

	}*/
}

