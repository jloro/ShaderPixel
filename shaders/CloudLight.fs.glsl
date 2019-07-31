#version 330 core
out vec4 FragColor;

in mat4 invertViewMat;
in mat4 invertProjMat;

uniform vec2 uResolution;//Resolution of screen
uniform float uFov;//Fov in radians
uniform vec3 uCamPos;//Position of camera in world
uniform vec2 uRotation;//Rotation of the camera
uniform vec3 uDir;//Dir of the camera
uniform vec3 uUp;//Dir of the camera
uniform float uGlobalTime;
uniform vec3 uOrigin;
uniform sampler2D uNoise;

const int MAX_MARCHING_STEPS = 300;
const float MIN_DIST = 0.0f;
const float MAX_DIST = 100.0f;
const float EPSILON = 0.001f;
const float VOLUME_STEP_LIGHT = 0.1;
const float VOLUME_DENSITY = 0.05;
const float LIGHT_INTESITY = 0.03;

struct obj
{
	float	d;
	vec3	rgb;
	bool	volumetric;
	float	r;
	vec3	pos;
};

obj		initSphere(bool volumetric, vec3 pos, float r, vec3 rgb)
{
	obj	sphere;

	sphere.volumetric = volumetric;
	sphere.pos = pos;
	sphere.r = r;
	sphere.rgb = rgb;
	return sphere;
}
obj sphereVolumetric = initSphere(true, vec3(0.0f), 1.0f, vec3(1.0f));
obj sphereObstacle = initSphere(false, vec3(1.5f, 0.0f, 0.0f), 0.1f, vec3(0.0f, 0.0f, 1.0f));
obj sphereSun = initSphere(false, vec3(2.0, cos(uGlobalTime), 0.0f), 0.1f, vec3(1.0f, 1.0f, 0.0f));

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


obj unionSDF(obj A, obj B) {
	if (A.d < B.d)
		return A;
	else
		return B;
}

float sphereSDF(vec3 samplePoint, vec3 o, float r) {
	return length(samplePoint - o) - r;
}


obj SceneSDF(vec3 p) {
	p -= uOrigin;
	sphereVolumetric.d = sphereSDF(p, sphereVolumetric.pos, sphereVolumetric.r);
	sphereObstacle.d = sphereSDF(p, sphereObstacle.pos, sphereObstacle.r);
	sphereSun.d = sphereSDF(p, sphereSun.pos, sphereSun.r);
	return unionSDF(sphereVolumetric, unionSDF(sphereObstacle, sphereSun));
}

float SceneSDFShadow(vec3 p) {
	p -= uOrigin;
	float d = sphereSDF(p, sphereObstacle.pos, sphereObstacle.r);
	return d;
}

float sampleVolume(vec3 p, float mult)
{
	p -= uOrigin;
	return cloudNoise(5, p, vec3(0.0, 0.125, 0.2) * uGlobalTime) * mult;
}

float volumeAbsorption(float lightIntensity, float accumDensityToPoint) {
	return max(0.0, lightIntensity - accumDensityToPoint);
}

const int maxStep = 100;
const float step = 0.05f;
float RayMarchToLight(vec3 ray, vec3 origin, float end)
{
	float density = 0.0f;
	float d, t = 0;
	vec3 pos;

	for (float t = 0.0f; t < end;)
	{
		pos = origin + ray * t;
		d = SceneSDFShadow(pos);

		if (d < EPSILON)
		{
			density += sampleVolume(pos, 5) * min(-d, VOLUME_STEP_LIGHT);
			t += VOLUME_STEP_LIGHT;
		}
		else
			t += d;

		if (density >= 1.0) return 1.0;
	}
	return density;
}

vec4 RayMarche(vec3 ray, vec3 origin)
{
	vec3 lightPos = sphereSun.pos + uOrigin;
	vec4 sum = vec4(0.0f);
	float t = 0.0f;
	vec3 pos;
	float accumDensity = 0.0f, brightness = 0.0f;
	float transmitance = 0.0f;
	obj res;
	vec3 col = vec3(1.0f);
	for (int i = 0; i < maxStep; i++)
	{
		pos = origin + ray * t;
		res = SceneSDF(pos);

		if (res.d < EPSILON)
		{
			if (!res.volumetric && transmitance == 0.0f)
				return vec4(res.rgb, 1.0f);
			else if (!res.volumetric)
				col = res.rgb;
			else
			{
				float densityAtPos = sampleVolume(pos, VOLUME_DENSITY);
				accumDensity += densityAtPos * min(-res.d, step);
				vec3 lightDir = vec3(lightPos - pos);
				float accumDensityToLight = RayMarchToLight(normalize(lightDir), pos, length(lightDir));
				float pointBrightness = volumeAbsorption(LIGHT_INTESITY, accumDensityToLight);
				brightness += volumeAbsorption(pointBrightness, accumDensity);
				t += step;
				transmitance += densityAtPos * (1 - transmitance);
			}
		}
		else
			t += res.d;

		if (brightness >= 1.0f || transmitance > 0.99f)
			return vec4(col, transmitance);
	}
	return vec4(col * brightness, transmitance);
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

	vec4 col = RayMarche(ray, uCamPos);

	FragColor = col;
}


