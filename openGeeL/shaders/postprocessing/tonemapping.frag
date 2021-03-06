#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform float exposure;


void main() { 
	vec3 hdr = texture(image, TexCoords).rgb;
	vec3 result = vec3(1.0f) - exp(-hdr * exposure);
	color = vec4(result, 1.0f);
}