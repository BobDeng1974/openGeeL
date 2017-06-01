#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texCoords;

out vec3 vNormal;
out vec3 vPosition;
out vec2 vTexCoords;


void main {
	vNormal = norm;
	vPosition = position;
	vTexCoords = texCoords;
		
	gl_Position = vec3(position, 1.f);
}