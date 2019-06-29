#version 330 core
out vec4 FragColor;

in mat4 viewMat;
in mat4 mvp;

uniform vec2	uResolution;//Resolution of screen
uniform float	uFov;//Fov in radians
uniform vec3	uCamPos;//Position of camera in world
uniform vec2	uRotation;//Rotation of the camera
uniform vec3	uDir;//Dir of the camera
uniform vec3	uUp;//Dir of the camera
uniform float	uGlobalTime;

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0f;
const float MAX_DIST = 100.0f;
const float EPSILON = 0.0001f;

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

float cubeSDF(vec3 p) {
    // If d.x < 0, then -1 < p.x < 1, and same logic applies to p.y, p.z
    // So if all components of d are negative, then p is inside the unit cube
    vec3 d = abs(p) - vec3(1.0, 1.0, 1.0);

    // Assuming p is inside the cube, how far is it from the surface?
    // Result will be negative or zero.
    float insideDistance = min(max(d.x, max(d.y, d.z)), 0.0);

    // Assuming p is outside the cube, how far is it from the surface?
    // Result will be positive or zero.
    float outsideDistance = length(max(d, 0.0));

    return insideDistance + outsideDistance;
}

float sphereSDF(vec3 samplePoint, vec3 o, float r) {
    return length(samplePoint - o) - r;
}

/**
 * Signed distance function for an XY aligned cylinder centered at the origin with
 * height h and radius r.
 */
float cappedCylinderSDF(vec3 p, float h, float r) {
    // How far inside or outside the cylinder the point is, radially
    float inOutRadius = length(p.xy) - r;

    // How far inside or outside the cylinder is, axially aligned with the cylinder
    float inOutHeight = abs(p.z) - h/2.0;

    // Assuming p is inside the cylinder, how far is it from the surface?
    // Result will be negative or zero.
    float insideDistance = min(max(inOutRadius, inOutHeight), 0.0);

    // Assuming p is outside the cylinder, how far is it from the surface?
    // Result will be positive or zero.
    float outsideDistance = length(max(vec2(inOutRadius, inOutHeight), 0.0));

    return insideDistance + outsideDistance;
}

/**
 * Signed distance function describing the scene.
 * 
 * Absolute value of the return value indicates the distance to the surface.
 * Sign indicates whether the point is inside or outside the surface,
 * negative indicating inside.
 */
float sceneSDF(vec3 samplePoint) 
{
    float displacement = sin(5.0 * samplePoint.x) 
    * sin(5.0 * samplePoint.y) * sin(5.0 * samplePoint.z) * (cos(uGlobalTime) / 2.7);

	vec3 samplePointCube = rotateX(-uGlobalTime) * samplePoint;
    float sphereDist = sphereSDF(samplePoint / 1.2, vec3(0.0f, 0.0f, 0.0f), 1.0f) * 1.2;
    float cubeDist = cubeSDF(samplePointCube);
    return sphereDist + displacement;
}

vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

/**
 * Lighting contribution of a single point light source via Phong illumination.
 *
 * The vec3 returned is the RGB color of the light's contribution.
 *
 * k_a: Ambient color
 * k_d: Diffuse color
 * k_s: Specular color
 * alpha: Shininess coefficient
 * p: position of point being lit
 * eye: the position of the camera
 * lightPos: the position of the light
 * lightIntensity: color/intensity of the light
 *
 * See https://en.wikipedia.org/wiki/Phong_reflection_model#Description
 */
vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = estimateNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));

    float dotLN = dot(L, N);
    float dotRV = dot(R, V);

    if (dotLN < 0.0) {
        // Light not visible from this point on the surface
        return vec3(0.0, 0.0, 0.0);
    }

    if (dotRV < 0.0) {
        // Light reflection in opposite direction as viewer, apply only diffuse
        // component
        return lightIntensity * (k_d * dotLN);
    }
    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

/**
 * Lighting via Phong illumination.
 *
 * The vec3 returned is the RGB color of that point after lighting is applied.
 * k_a: Ambient color
 * k_d: Diffuse color
 * k_s: Specular color
 * alpha: Shininess coefficient
 * p: position of point being lit
 * eye: the position of the camera
 *
 * See https://en.wikipedia.org/wiki/Phong_reflection_model#Description
 */
vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;

    vec3 light1Pos = vec3(4.0,
                          2.0,
                          4.0);
    vec3 light1Intensity = vec3(0.4, 0.4, 0.4);

    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity);

    return color;
}

float	rayMarche(float start, float end, vec3 ray, vec3 origin)
{
	float depth = start;
	float dist;
	for (int i = 0; i < MAX_MARCHING_STEPS; i++)
	{
		dist = sceneSDF(origin + depth * ray);
		if (dist < EPSILON)
			return depth;
		depth += dist;
		if (depth >= end)
			return end;
	}
	return end;
}


mat4 viewMatrix(vec3 eye, vec3 center, vec3 up) {
    // Based on gluLookAt man page
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(
        vec4(s, 0.0),
        vec4(u, 0.0),
        vec4(-f, 0.0),
        vec4(0.0, 0.0, 0.0, 1)
    );
}

vec3 rotateRay(vec3 ray)
{
	mat4 mat = viewMatrix(uCamPos, uCamPos + uDir, uUp);
	ray = (mat * vec4(ray, 0.0f)).xyz;
	return ray;
}

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size / 2.0;
    float z = size.y / tan(fieldOfView / 2.0);
    vec3 ray = normalize(vec3(xy, -z));
	return rotateRay(ray);
}

void main()
{
	vec3 ray = rayDirection(uFov, uResolution, gl_FragCoord.xy);

	float dist = rayMarche(MIN_DIST, MAX_DIST, ray, uCamPos);

    if (dist > MAX_DIST - EPSILON) {
        // Didn't hit anything
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        //FragColor = vec4(0.0, 0.0, 1.0, 1.0);
		return;
    }

    // The closest point on the surface to the eyepoint along the view ray
    vec3 p = uCamPos + dist * ray;

    vec3 K_a = vec3(0.2, 0.2, 0.2);
    vec3 K_d = vec3(0.7, 0.2, 0.2);
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;

    vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p, uCamPos);

    FragColor = vec4(color, 1.0);
}
