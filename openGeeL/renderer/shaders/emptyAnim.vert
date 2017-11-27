#version 430 core

layout (location = 0) in vec3 position;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4  weights;

uniform mat4 bones[50];

void main() {
	mat4 model = bones[boneIDs[0]] * weights[0];
    model     += bones[boneIDs[1]] * weights[1];
    model     += bones[boneIDs[2]] * weights[2];
    model     += bones[boneIDs[3]] * weights[3];

    gl_Position = model * vec4(position, 1.0f);
}  