#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform float gamma;
uniform sampler2D image;


void main() {
	color = texture(image, TexCoords).rgba;
	
	color.rgb = pow(color.rgb, vec3(1.0f / gamma));
	color.a = 1;
}
