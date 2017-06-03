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


void main() {
	unsigned int voxelIndex = gl_GlobalInvocationID.y * 1024 + gl_GlobalInvocationID.x;
	
	if(voxelIndex >= numVoxels)
	    return; //Should never happen

	//Dimensions of root node
	unsigned int dim = dimensions;
	uvec3 umin = uvec3(0, 0, 0);
	uvec3 umax = uvec3(dim, dim, dim);

	uvec4 vP = imageLoad(voxelPositions, int(voxelIndex));  //Position of voxel
	vec4 diffuse = imageLoad(voxelColors, int(voxelIndex)); //Color of voxel

	int nodeIndex = 0; //Set index to root node (0)
	unsigned int node = imageLoad(nodeIndicies, nodeIndex).r;

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

	imageAtomicRGBA8Avg(diffuse, nodeIndex, nodeColors);
}


vec4 convRGBA8ToVec4(in uint val) {
    return vec4( float( (val&0x000000FF) ), float( (val&0x0000FF00)>>8U),
	             float( (val&0x00FF0000)>>16U), float( (val&0xFF000000)>>24U) );
}

uint convVec4ToRGBA8(in vec4 val) {
    return ( uint(val.w)&0x000000FF)<<24U | (uint(val.z)&0x000000FF)<<16U | (uint(val.y)&0x000000FF)<<8U | (uint(val.x)&0x000000FF);
}

void imageAtomicRGBA8Avg(vec4 val, int coord, layout(r32ui) uimageBuffer buf) {
    val.rgb *= 255.0;
	val.a = 1;

	uint newVal = convVec4ToRGBA8( val );
	uint prev = 0;
	uint cur;
	
	while((cur = imageAtomicCompSwap( buf, coord, prev, newVal)) != prev) {
       prev = cur;
	   vec4 rval = convRGBA8ToVec4(cur);
	   rval.xyz = rval.xyz*rval.w;
	   vec4 curVal = rval +  val;
	   curVal.xyz /= curVal.w;
	   newVal = convVec4ToRGBA8(curVal);
	}
}