#version 330 core
layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 nnormal;
layout (location = 2) in vec2 texCoords;

out vec3 normal;
out vec3 fragPosition;
out vec2 textureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	
	vec4 localPosition = model * vec4(_position, 1.0f);

	normal = nnormal;
	fragPosition = vec3(localPosition);
	textureCoordinates = texCoords;

    gl_Position = projection * view * localPosition;
	
} 