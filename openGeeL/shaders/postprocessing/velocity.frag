#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D previousPosition;
uniform sampler2D currentPosition;

uniform mat4 projection;
uniform vec3 defaultOffset;
uniform vec3 origin;

vec4 transformToClip(vec3 vector);

void main() { 

	vec3 prevPos = texture(previousPosition, TexCoords).xyz;
	vec3 currPos = texture(currentPosition, TexCoords).xyz;

	//Use default offset if trying to blur over skybox
	if(length(prevPos) < 0.1f || length(currPos) < 0.1f) {
		color = vec4(defaultOffset, 1.f);
		return;
	}

	prevPos = transformToClip(prevPos).xyz;
	currPos = transformToClip(currPos).xyz;

	vec3 velocity = currPos - prevPos;

	color = vec4(velocity, 1.f);
}

//Transform to clip space
vec4 transformToClip(vec3 vector) {
	vec4 vecProj = vec4(vector, 1.f);
	vecProj = projection * vecProj;
	vecProj.xyz = vecProj.xyz / vecProj.w;
	vecProj.xyz = vecProj.xyz * 0.5 + 0.5;

	return vecProj;
}