#version 430 core

layout(location = 0) in vec3  position;
layout(location = 1) in vec3  norm;
layout(location = 2) in vec2  texCoordss;
layout(location = 3) in vec3  tangent;
layout(location = 4) in vec3  bitangent;
layout(location = 5) in ivec4 boneIDs;
layout(location = 6) in vec4  weights;

out vec3 normal;
out vec3 fragPosition;
out vec2 texCoords;
out mat3 TBN;

uniform mat4 bones[50];
uniform mat4 projection;
uniform mat4 view;


void main() {
	mat4 model = bones[boneIDs[0]] * weights[0];
	model	  += bones[boneIDs[1]] * weights[1];
	model	  += bones[boneIDs[2]] * weights[2];
	model	  += bones[boneIDs[3]] * weights[3];

	vec4 frag = model * vec4(position, 1.f);
	vec4 localPosition = view * frag;

	mat3 normalMat = mat3(model);
	normal = norm;
	fragPosition = frag.xyz;
	texCoords = texCoordss;

	vec3 T = normalize(normalMat * tangent);
	vec3 B = normalize(normalMat * bitangent);
	vec3 N = normalize(normalMat * norm);
	TBN = mat3(T, B, N);

	gl_Position = projection * localPosition;
}

