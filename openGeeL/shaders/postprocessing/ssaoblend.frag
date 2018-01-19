#version 430 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;

void main() { 
	color = vec4(0.f, 0.f, 0.f, texture(image, TexCoords).r);
}