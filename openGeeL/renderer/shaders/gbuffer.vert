#version 330 core

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 nnormal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout (std140) uniform cameraMatrices {
    mat4 projection;
    mat4 view;
	vec3 camPosition;
};

out vec3 normal;
out vec3 fragPosition;
out vec2 textureCoordinates;
out mat3 TBN;

uniform mat4 model;

void main() {	
	vec4 localPosition = model * vec4(_position, 1.0f);

	normal = transpose(inverse(mat3(model))) * nnormal;
	fragPosition = vec3(localPosition);
	textureCoordinates = texCoords;

	mat3 normalMat = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMat * tangent);
	vec3 B = normalize(normalMat * bitangent);
	vec3 N = normalize(normalMat * nnormal);
	mat3 TBN = mat3(T, B, N);

    gl_Position = projection * view * localPosition;
} 