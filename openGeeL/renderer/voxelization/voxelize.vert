#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texCoords;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoords;

uniform mat4 model;

void main() {
	vec4 pos = model * vec4(position, 1.f);
	vPosition = pos.xyz;
	vNormal = norm;
	//vNormal = normalize(transpose(inverse(mat3(model))) * norm);
	vTexCoords = texCoords;
		
	gl_Position = pos;
}