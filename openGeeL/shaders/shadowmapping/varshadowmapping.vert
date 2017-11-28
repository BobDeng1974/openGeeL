#version 430 core

layout (location = 0) in vec3 position;

out vec4 fragPosition;

uniform mat4 lightTransform;
uniform mat4 model;

void main() {

    gl_Position = lightTransform * model * vec4(position, 1.0f);
	fragPosition = gl_Position;
}  