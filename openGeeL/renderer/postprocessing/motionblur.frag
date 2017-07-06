#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D prevFrame;

uniform float strength;

void main() {
	vec3 current = texture(image, TexCoords).rgb; 
	vec3 previous = texture(prevFrame, TexCoords).rgb; 
	vec3 result = (1.f - strength) * current + strength * previous;

	if(TexCoords.x < 0.5f)
		color = vec4(result, 1.f);
	else
		color = vec4(result, 1.f);
	//color = vec4(vec3(current.r), 1.f);
}