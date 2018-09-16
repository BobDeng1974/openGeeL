#version 430

layout (local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

uniform int nodeOffset;
uniform int allocOffset;
uniform int numNodes;

layout (binding = 0, offset = 0) uniform atomic_uint nodeCounter;

uniform layout(binding = 0, r32ui) uimageBuffer nodeIndicies;


void main() {
	unsigned int index = gl_GlobalInvocationID.x;

	//Filter out abundant calls of work group
	if(index < numNodes) {
		unsigned int childIndex = imageLoad(nodeIndicies, nodeOffset + int(index)).r;
	
		//Child flag is set and therefore child nodes will be allocated
		//and their position linked to parent node
		if((childIndex & 0x80000000) != 0) {
			unsigned int offset;
			offset = atomicCounterIncrement(nodeCounter); //Generate start index
			offset *= 8;								  //Add offset for 8 child nodes									
			offset += allocOffset;						  //Offset to free allocation space
			offset |= 0x80000000;						  //Add child flag again

			imageStore(nodeIndicies, nodeOffset + int(index), uvec4(offset, 0, 0, 0));
		}
	}
}