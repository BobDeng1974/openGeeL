#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightTransform;
uniform mat4 model;

void main()
{
    gl_Position = lightTransform * model * vec4(position, 1.0f);
	//gl_Position = vec4(5, 1, 2, 0);
}  