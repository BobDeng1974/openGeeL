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
out float clipDepth;

uniform mat4 modelView;
uniform mat3 transInvModelView;

void main() {	
	vec4 localPosition = modelView * vec4(position, 1.0f);

	normal = normalize(transInvModelView * norm);
	fragPosition = vec3(localPosition);
	textureCoordinates = texCoords;

	vec3 T = normalize(transInvModelView * tangent);
	vec3 B = normalize(transInvModelView * bitangent);
	vec3 N = normalize(transInvModelView * norm);
	TBN = mat3(T, B, N);

    gl_Position = projection * localPosition;
	clipDepth = gl_Position.z;
} 