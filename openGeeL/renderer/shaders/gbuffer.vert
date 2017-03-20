#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
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
	mat4 modelView = view * model;
	vec4 localPosition = modelView * vec4(position, 1.0f);

	normal = transpose(inverse(mat3(modelView))) * norm;
	fragPosition = vec3(localPosition);
	textureCoordinates = texCoords;

	mat3 normalMat = transpose(inverse(mat3(modelView)));
	vec3 T = normalize(normalMat * tangent);
	vec3 B = normalize(normalMat * bitangent);
	vec3 N = normalize(normalMat * norm);
	TBN = mat3(T, B, N);

    gl_Position = projection * localPosition;
} 