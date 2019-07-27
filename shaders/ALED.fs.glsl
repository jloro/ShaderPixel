#version 330 core
out vec4 FragColor;

in mat4 viewMat;
in mat4 mvp;
in mat4 proj;
in vec3 tt;

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
const float VOLUME_STEP_LIGHT = 0.03;
const float VOLUME_DENSITY = 0.04;
const float LIGHT_INTESITY = 0.03;

float hash( float n ) { return fract(sin(n)*753.5453123); }
float noise( in vec3 x )
{
	   vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);

	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
	vec2 rg = texture( uNoise, (uv+0.5)/256.0, -100.0 ).yx;
	return mix( rg.x, rg.y, f.z );
}
float cloudNoise(float scale,in vec3 p, in vec3 dir)
{
	vec3 q = p + dir;
    float f;
	f  = 0.50000*noise( q ); q = q*scale*2.02 + dir;
	f += 0.25000*noise( q ); q = q*2.03 + dir;
    f += 0.12500*noise( q ); q = q*2.01 + dir;
    f += 0.06250*noise( q ); q = q*2.02 + dir;
    f += 0.03125*noise( q );
    return f;
}


float unionSDF(float distA, float distB) {
    return min(distA, distB);
}

float sphereSDF(vec3 samplePoint, vec3 o, float r) {
    return length(samplePoint - o) - r;
}

float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}


float SceneSDF(vec3 p) {
	p -= uOrigin;
	float d = sphereSDF(p, vec3(0, 0, 0), 1.0f);
	return d;
}

float SceneSDFf(vec3 p) {
	p -= uOrigin;
	float d = sphereSDF(p, vec3(0, 0, 0), 0.5f);
	return d;
}
vec3 CalcNormal(vec3 p) {
    return normalize(vec3(
        SceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - SceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        SceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - SceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        SceneSDF(vec3(p.x, p.y, p.z + EPSILON)) - SceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

float layeredNoise(in vec3 x) {
	//x += vec3(10.0 + cos(uGlobalTime * .1) * 5.0, 5.0* sin(uGlobalTime * .01), 6.0* -cos(uGlobalTime * .01));
    return 0.6*noise(x*5.0) + 0.4*noise(x*10.0) + 0.2*noise(x*16.0) - 0.2;
}

float sampleVolume(vec3 p, float mult)
{
	return layeredNoise(p) * mult;
}

float volumeAbsorption(float lightIntensity, float accumDensityToPoint) {
    return max(0.0, lightIntensity - accumDensityToPoint);
}

const int maxStep = 50;
const float step = 0.03f;
float	RayMarchToLight(vec3 ray, vec3 origin)
{
	float density = 0.0f;
	float d, t =0.0f;
	vec3 pos;

	for (int i = 0; i < 50;i++)
	{
		pos = origin + ray * t;
		d = SceneSDFf(pos);

		if (d < EPSILON)
		{
			density += sampleVolume(pos, .3)* min(-d, VOLUME_STEP_LIGHT);
			t += VOLUME_STEP_LIGHT;
		}
		else
			t += d;

		if (density >= 1.0f)
			return 1.0f;
	}
	return density;
}
float	RayMarche(vec3 ray, vec3 origin)//vec2(distAccum, 1st p)
{
	vec3 lightPos = vec3(2*cos(uGlobalTime), 3.0, 2*sin(uGlobalTime));
	//vec3 lightPos = vec3(2, 3.0, 2);
	vec4 sum = vec4(0.0f), col;
	float t = 0.0f, d;
	vec3 pos;
	float accumDensity = 0.0f;
	float brightness = 0.0f;
	for (int i = 0; i < maxStep; i++)
	{
		pos = origin + ray * t;
		d = SceneSDF(pos);

		if (d < EPSILON)
		{
			float densityAtPos = sampleVolume(pos, VOLUME_DENSITY);
			accumDensity += densityAtPos * min(-d, step);
			vec3 lightDir = vec3(lightPos - pos);
			float accumDensityToLight = RayMarchToLight(lightPos, pos);
			float pointBrightness = volumeAbsorption(LIGHT_INTESITY, accumDensityToLight);
			brightness += volumeAbsorption(pointBrightness, accumDensity);
			t += step;
		}
		else
			t += d;
		if (brightness >= 1.0f)
			return 1.0f;
	}
	return brightness;
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

	float dist = RayMarche(ray, uCamPos);

//	FragColor = vec4(1.0f) * dist;
	FragColor = vec4(vec3(1.0f) * dist, 1.0f);
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

