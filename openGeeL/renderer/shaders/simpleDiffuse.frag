#version 330 core

in vec2 textureCoordinates;

out vec4 color;

uniform sampler2D diffuse1;

void main()
{    
	color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    //color = vec4(texture(diffuse, textureCoordinates));
}