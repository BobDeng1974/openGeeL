#version 430 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform sampler2D image2;


void main() { 
	color = texture(image, TexCoords);

}