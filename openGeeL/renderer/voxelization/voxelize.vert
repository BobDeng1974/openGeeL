#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoords;
out mat3 gTBN;

uniform mat4 model;

void main() {
	vec4 pos = model * vec4(position, 1.f);
	vPosition = pos.xyz;
	vNormal = norm;
	//vNormal = normalize(transpose(inverse(mat3(model))) * norm);
	vTexCoords = texCoords;

	mat3 normalMat = mat3(model);
	vec3 T = normalize(normalMat * tangent);
	vec3 B = normalize(normalMat * bitangent);
	vec3 N = normalize(normalMat * norm);
	gTBN = mat3(T, B, N);
		
	gl_Position = pos;
}