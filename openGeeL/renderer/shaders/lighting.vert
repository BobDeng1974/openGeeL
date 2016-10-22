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

out vec4 spotLightTransforms[5];
out vec4 direLightTransforms[5];

uniform int dlCount;
uniform int slCount;

uniform mat4 model;
uniform mat4 spotLightMatrix[5];
uniform mat4 direLightMatrix[5];

void main() {
	
	vec4 localPosition = model * vec4(_position, 1.0f);

	normal = transpose(inverse(mat3(model))) * nnormal;
	fragPosition = vec3(localPosition);

	//Write transforms for shadow mapping. This is done here in the 
	//vertex shader because these transforms are equal for every fragment 
	for(int i = 0; i < slCount; i++)
		spotLightTransforms[i] = spotLightMatrix[i] * localPosition;
	for(int i = 0; i < dlCount; i++)
		direLightTransforms[i] = direLightMatrix[i] * localPosition;

	textureCoordinates = texCoords;
	cameraPosition = camPosition;

    gl_Position = projection * view * localPosition;
} 