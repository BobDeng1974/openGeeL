#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;

uniform int numVoxels;
uniform int level;
uniform int dimensions;

uniform layout (binding = 0, rgb10_a2ui) uimageBuffer voxelPositions;
uniform layout (binding = 1, r32ui) uimageBuffer nodeIndicies;


void main() {
	unsigned int voxelIndex = gl_GlobalInvocationID.y * 1024 + gl_GlobalInvocationID.x;
	
	if(voxelIndex >= numVoxels)
	    return; //Should never happen

	//Dimensions of root node
	unsigned int dim = dimensions;
	uvec3 umin = uvec3(0, 0, 0);
	uvec3 umax = uvec3(dim, dim, dim);

	uvec4 vP = imageLoad(voxelPositions, int(voxelIndex)); //Position of voxel
	
	int nodeIndex = 0; //Set index to root node (0)
	unsigned int node = imageLoad(nodeIndicies, nodeIndex).r;


	//Iterate through the levels and find suitable child node for voxel 
	for(int i = 0; i < level; i++) {
		nodeIndex = int(node & 0x7FFFFFFF); //Remove child flag

		dim /= 2;
		uvec3 box; //Spacial index of subnode
		box.x = uint(vP.x > (umin.x + dim));
		box.y = uint(vP.y > (umin.y + dim));
		box.z = uint(vP.y > (umin.z + dim));

		umin += box * dim;

		unsigned int childIndex; //Spacial position translated into index
		childIndex = box.x + 4 * box.y + 2 + box.z;
		nodeIndex += int(childIndex);

		node = imageLoad(nodeIndicies, nodeIndex).r;
	}


	node |= 0x80000000;  //Add child flag again
	imageStore(nodeIndicies, nodeIndex, uvec4(node, 0, 0, 0));
}