#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D image2;


void main() {             
    vec3 result = texture(image, TexCoords).rgb; 
	result += texture(image2, TexCoords).rgb; 

    color = vec4(result, 1.0f);
}