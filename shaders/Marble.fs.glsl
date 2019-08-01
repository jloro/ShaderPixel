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
uniform samplerCube skybox;

const int MAX_MARCHING_STEPS = 300;
const float MIN_DIST = 0.0f;
const float MAX_DIST = 100.0f;
const float EPSILON = 0.001f;

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
vec2 sphereIntersect(in vec3 origin, in vec3 ray, in vec3 o, float r)
{
    vec3 oc = origin - o;
    float b = dot( oc, ray );
    float c = dot( oc, oc ) - r*r;
    float h = b*b - c;
    if( h<0.0 ) return vec2(-1.0);
    h = sqrt( h );
    return vec2( -b-h, -b+h );
}

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
float sphereSDF(vec3 samplePoint, vec3 o, float r) {
	return length(samplePoint - o) - r;
}


float unionSDF(float A, float B) {
	return min(A, B);
}
float torusSDF(vec3 p, vec3 o, vec2 r, mat3 rot)//r = vec2(big radius, little radius)
{
	p -= o;
	p *= rot;
	vec2 q = vec2(length(p.xz)-r.x,p.y);
	return length(q)-r.y;
}

float map(vec3 p)
{
	p -= uOrigin;
	float tor1 = torusSDF(p, vec3(0), vec2(.8, .05), mat3(1.0f));
	float tor2 = torusSDF(p, vec3(0), vec2(.8, .05), rotateX(radians(30)));
	float tor3 = torusSDF(p, vec3(0), vec2(.8, .05), rotateX(radians(60)));
	float tor4 = torusSDF(p, vec3(0), vec2(.8, .05), rotateX(radians(90)));
	float tor5 = torusSDF(p, vec3(0), vec2(.8, .05), rotateX(radians(120)));
	float tor6 = torusSDF(p, vec3(0), vec2(.8, .05), rotateX(radians(150)));
	return unionSDF(tor1, unionSDF(tor2, unionSDF(tor3, unionSDF(tor4, unionSDF(tor5, tor6)))));
}
vec4 raymarch( in vec3 ro, in vec3 rd, in vec2 tminmax )
{
    float t = tminmax.x;
	vec3 col = vec3(.1, .1, .7);
    for( int i=0; i<300; i++ )
	{
		if (t > tminmax.y)
			return vec4(col, tminmax.y - tminmax.x);
		vec3 pos = ro + rd * t;
		float d = map(pos);
		if (d < EPSILON)
		{
			col = vec3(.7, .1, .2);
			break;
		}
		t += d;
    }    
    return vec4(col, t - tminmax.x);
}


vec3 sphereNormal(vec3 p) {
	return normalize(p - uOrigin);
}

vec3 phongContribForLight(vec3 k_d, vec3 p, vec3 eye,
		vec3 lightPos, vec3 lightIntensity, vec3 N, vec3 L, vec3 k_s, float alpha) {
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));

    float dotLN = max(dot(L, N), 0.0);
    float dotRV = max(dot(R, V), 0.0);

    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float shininess, vec3 p, vec3 eye) {
	const vec3 ambientLight = .5 * vec3(1.0, 1.0, 1.0);
	vec3 color = ambientLight * k_a;

	vec3 lightPos = vec3(0, 0, 2);
	lightPos += uOrigin;
	vec3 lightIntensity = vec3(1.0, 1.0, 1.0);

	vec3 N = sphereNormal(p);
	vec3 L = normalize(lightPos - p);

	color += phongContribForLight(k_d, p, eye, lightPos, lightIntensity, N, L, k_s, shininess);

	return color;
}
//calc NDC from gl_fragCoosphereIntersect()rd : https://www.khronos.org/opengl/wiki/Compute_eye_space_from_window_space

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

	vec2 res = sphereIntersect(uCamPos, ray, vec3(0) + uOrigin, 1.0f);
	if (res.x >=0 || res.y >= 0)
	{
		vec4 t = raymarch(uCamPos, ray, res);
		vec3 lightCol = vec3(.4, .4, .4);
		float opacity = exp(-t.w * .5);
		vec3 re = lightCol * (1 - opacity) + t.rgb * opacity;

		vec3 p = uCamPos + res.x * ray;

		vec3 K_d = re;
		vec3 K_a = K_d;
		vec3 K_s = vec3(1.0f);
		float shininess = 60.f;

		vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p, uCamPos);
		vec3 nor=p - uOrigin;
        nor = reflect(ray, nor);
        float fre = pow(.3+ clamp(dot(nor,ray),0.0,1.0), 3. )*0.6;
        color += texture(skybox, nor).rgb * fre;
		FragColor = vec4(color, 1.0f);
	}
	else
		FragColor = vec4(0.0);
}


