#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

uniform int allocOffset;
uniform int numNodes;

uniform layout(binding = 0, r32ui) coherent uimageBuffer nodeIndicies;
uniform layout(binding = 1, r32ui) coherent uimageBuffer nodeDiffuse;


void main() {
	unsigned int index = gl_GlobalInvocationID.y * 1024 + gl_GlobalInvocationID.x;

	//Filter out abundant calls of work group
	if(index < numNodes) {
		imageStore(nodeIndicies, int(allocOffset + index), uvec4(0));
		imageStore(nodeDiffuse,  int(allocOffset + index), uvec4(0));
	}
}