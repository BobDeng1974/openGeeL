#version 330 core

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 nnormal;
layout (location = 2) in vec2 texCoords;

layout (std140) uniform cameraMatrices
{
    mat4 projection;
    mat4 view;
	vec3 camPosition;
};

out vec3 normal;
out vec3 fragPosition;
out vec2 textureCoordinates;
out vec3 cameraPosition;
out vec4 fragPositionLightSpace;

uniform mat4 model;
uniform mat4 lightTransform;


void main() {
	
	vec4 localPosition = model * vec4(_position, 1.0f);

	normal = transpose(inverse(mat3(model))) * nnormal;
	fragPosition = vec3(localPosition);
	fragPositionLightSpace = lightTransform * localPosition;

	textureCoordinates = texCoords;
	cameraPosition = camPosition;

    gl_Position = projection * view * localPosition;
} 