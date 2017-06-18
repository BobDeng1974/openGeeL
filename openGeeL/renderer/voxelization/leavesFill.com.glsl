# version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

uniform layout(binding = 0, r32ui) uimageBuffer nodeIndicies;
uniform layout(binding = 1, r32ui) uimageBuffer nodeColors;
uniform layout(binding = 2, rgb10_a2ui) uimageBuffer voxelPositions;
uniform layout(binding = 3, rgba8) imageBuffer voxelColors;

uniform int dimensions;
uniform int level;
uniform int numVoxels;

//Atomic average method from OpenGL Insight chapter 22
void imageAtomicRGBA8Avg(vec4 val, int coord, layout(r32ui) uimageBuffer buf);
vec4 convRGBA8ToVec4(in uint val);
uint convVec4ToRGBA8(in vec4 val);


void main() {
	unsigned int voxelIndex = gl_GlobalInvocationID.y * 1024 + gl_GlobalInvocationID.x;
	
	//Filter out abundant calls of work group
	if(voxelIndex >= numVoxels) return;

	//Dimensions of root node
	unsigned int dim = dimensions;
	uvec3 umin = uvec3(0, 0, 0);
	uvec3 umax = uvec3(dim, dim, dim);

	uvec4 position = imageLoad(voxelPositions, int(voxelIndex));  //Position of voxel
	vec4 diffuse = imageLoad(voxelColors, int(voxelIndex)); //Color of voxel

	int nodeIndex = 0; //Set index to root node (0)
	unsigned int node = imageLoad(nodeIndicies, nodeIndex).r;

	for(int i = 0; i < level; i++) {
		nodeIndex = int(node & 0x7FFFFFFF); //Remove child flag

		dim /= 2;
		//Spacial index of subnode
		uvec3 box = clamp(ivec3(1 + position.xyz - umin - dim), 0, 1);
		umin += box * dim;

		//Spacial position translated into index
		unsigned int childIndex = box.x + 4 * box.y + 2 * box.z;
		nodeIndex += int(childIndex);

		node = imageLoad(nodeIndicies, nodeIndex).r;
	}

	//Write voxlel colors into leaf nodes
	imageAtomicRGBA8Avg(diffuse, nodeIndex, nodeColors);

	//Mark leaf node as non-empty
	node |= 0x80000000;
	imageStore(nodeIndicies, nodeIndex, uvec4(node, 0, 0, 0));
}


vec4 convRGBA8ToVec4(in uint val) {
    return vec4( float((val&0x000000FF)), float((val&0x0000FF00)>>8U),
	             float((val&0x00FF0000)>>16U), float((val&0xFF000000)>>24U) );
}

uint convVec4ToRGBA8(in vec4 val) {
    return (uint(val.w)&0x000000FF)<<24U | (uint(val.z)&0x000000FF)<<16U | (uint(val.y)&0x000000FF)<<8U | (uint(val.x)&0x000000FF);
}

void imageAtomicRGBA8Avg(vec4 val, int coord, layout(r32ui) uimageBuffer buf) {
    val.rgb *= 255.0;
	val.a = 1;

	uint newVal = convVec4ToRGBA8(val);
	uint prev = 0;
	uint cur;
	
	while((cur = imageAtomicCompSwap(buf, coord, prev, newVal)) != prev) {
       prev = cur;
	   vec4 rval = convRGBA8ToVec4(cur);
	   rval.xyz = rval.xyz*rval.w;
	   vec4 curVal = rval + val;
	   curVal.xyz /= curVal.w;
	   newVal = convVec4ToRGBA8(curVal);
	}
}