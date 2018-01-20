#version 430 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

out vec4 fragPosition;

uniform mat4 lightTransforms[6];

void main() {

	for(int face = 0; face < 6; face++) {
		gl_Layer = face;
		for(int i = 0; i < 3; i++) {
			fragPosition = gl_in[i].gl_Position;
			gl_Position = lightTransforms[face] * fragPosition;
			EmitVertex();
		}
	
		EndPrimitive();	
	}
}
