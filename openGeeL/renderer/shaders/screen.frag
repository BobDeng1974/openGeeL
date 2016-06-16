#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D screenTexture;


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
    //color = texture(screenTexture, TexCoords);
	color = texture(screenTexture, TexCoords);
    //float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    //color = vec4(average, average, average, 1.0);

	vec3 col = vec3(color);
	
	col = rgb2hsv(col);

	col.g = col.g * 0.1f;
	col = hsv2rgb(col);

	color = vec4(col, 1.f);

}