#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texCoordss;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;


out vec3 normal;
out vec3 fragPosition;
out vec2 texCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() {
	vec4 frag = model * vec4(position, 1.f);
	vec4 localPosition = view * frag;

	normal = norm;
	fragPosition = frag.xyz;
	texCoords = texCoordss;

	mat3 normalMat = mat3(model);
	vec3 T = normalize(normalMat * tangent);
	vec3 B = normalize(normalMat * bitangent);
	vec3 N = normalize(normalMat * norm);
	TBN = mat3(T, B, N);

    gl_Position = projection * localPosition;
} 