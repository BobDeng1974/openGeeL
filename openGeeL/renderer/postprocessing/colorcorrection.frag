#version 330 core

uniform float r, g, b, h, s, v;
uniform vec2 direction;
uniform vec3 distortion;

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;


vec3 rgb2hsv(vec3 c) {

    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c) {

    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


void main() { 
	//color = texture(image, TexCoords);
	//vec3 col = vec3(color);

	vec3 col = vec3(texture(image, TexCoords + direction * distortion.r).r,
		texture(image, TexCoords + direction * distortion.g).g,
		texture(image, TexCoords + direction * distortion.b).b);
	
	col.r *= r;
	col.g *= g;
	col.b *= b;

	col = rgb2hsv(col);
	col.r *= h;
	col.g *= s;
	col.b *= v;

	col = hsv2rgb(col);
	color = vec4(col, 1.f);
}