#version 430 core

in vec3 TexCoords;

out vec4 color;

uniform samplerCube skybox;
uniform float brightnessScale = 1.f;

void main() {
	color = texture(skybox, TexCoords) * brightnessScale;
}