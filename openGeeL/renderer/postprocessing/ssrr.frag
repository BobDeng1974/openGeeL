#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;

uniform mat4 projection;


void main() {
	vec3 result = texture(image, TexCoords).rgb; 
	vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
	vec3 normal  = texture(gNormalMet, TexCoords).rgb;

	color = vec4(result, 1.0f);
}