#version 330 core
out vec4 FragColor;
  
uniform vec2	uResolution;//Resolution of screen
uniform float	uFov;//Fov in radians
uniform vec3	uCamPos;//Position of camera in world
uniform vec2	uRotation;//Rotation of the camera
uniform vec3	uDir;//Dir of the camera
uniform vec3	uUp;//Dir of the camera
uniform float	uGlobalTime;

mat2 rotate(float theta) {
    float c = cos(theta);
    float s = sin(theta);

    return mat2(
        vec2(c, -s),
        vec2(s, c)
    );
}
vec2 fold(vec2 p, float ang){
    vec2 n = vec2(cos(-ang), sin(-ang));
    p -= 2.0f * min(0.0f, dot(p, n)) * n;
    return p;
}

float d2hline(vec2 p){
    p.x -= max(0.0f, min(1.0f, p.x));
    return length(p);
}

vec2 gen_fold(vec2 pt) {
    pt = fold(pt,-2.9f);
	pt *= rotate(sin(uGlobalTime));
    pt = fold(pt, 0.9f);
    pt.y += sin(uGlobalTime) / 2.0f + 0.5f;
    pt = fold(pt, -1.0f);
    return pt;
}
vec2 gen_curve(vec2 pt) {
    for(int i = 0; i < 9; i++){
        pt *= 2.0f;
        pt.x -= 1.0f;
		pt *= rotate(cos(uGlobalTime));
        pt = gen_fold(pt);
    }
    return pt;
}
vec3 color(vec2 pt) {
    pt -= vec2(-0.630,0.000);
    pt *= 0.8f;
    pt = gen_curve(pt);
	float t = d2hline(pt);
    return vec3(t, t * 2.0f, t * 5.0f / 4.0f) * (cos(uGlobalTime) / 2.0f + 0.5f);
}
void main(){
	float imageAspectRatio = uResolution.x / uResolution.y;
	vec4 viewport = vec4(0, 0, uResolution.x, uResolution.y);
	vec2 uv = ((2.0 * gl_FragCoord.xy) - (2.0 * viewport.xy)) / (viewport.zw) - 1;

	FragColor = vec4(1.0f - color(uv), 1.0f);
}
