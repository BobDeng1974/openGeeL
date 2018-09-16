#version 430

layout (local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

uniform int nodeOffset;
uniform int numNodes;

uniform layout(binding = 0, r32ui) uimageBuffer nodeIndicies;
uniform layout(binding = 1, r32ui) uimageBuffer nodeDiffuse;


vec4 convRGBA8ToVec4(in uint val);
uint convVec4ToRGBA8(in vec4 val);

vec4 getColor(int nodeIndex);


void main() {
	unsigned int index = gl_GlobalInvocationID.x;

	//Filter out abundant calls of work group
	if(index < numNodes) {
		unsigned int childIndex = imageLoad(nodeIndicies, nodeOffset + int(index)).r;
	
		//Child flag is set and therefore node needs to be mipmapped
		if((childIndex & 0x80000000) != 0) {
			int nodeIndex = int(childIndex & 0x7FFFFFFF); //Remove child flag

			float count = 0.f;
			vec4 color = vec4(0.f);
			for(int i = 0; i < 8; i++) {
				unsigned int node = imageLoad(nodeIndicies, nodeIndex).r;

				if((node & 0x80000000) != 0) {
					color += getColor(nodeIndex);
					count += 1.f;
				}
			
				nodeIndex++;
			}

			color.rgb /= count;
			color.a /= 8.f; 

			uint rgba8 = convVec4ToRGBA8(color * 255.f);
			imageStore(nodeDiffuse, nodeOffset + int(index), uvec4(rgba8, 0, 0, 0));
		}
	}
}


vec4 getColor(int nodeIndex) {
	unsigned int rgb8 = imageLoad(nodeDiffuse, nodeIndex).r;
	return convRGBA8ToVec4(rgb8).rgba / 255;
}


vec4 convRGBA8ToVec4(in uint val) {
    return vec4(float((val&0x000000FF)), float((val&0x0000FF00)>>8U),
	    float((val&0x00FF0000)>>16U), float((val&0xFF000000)>>24U));
}

uint convVec4ToRGBA8(in vec4 val) {
    return (uint(val.w)&0x000000FF)<<24U | (uint(val.z)&0x000000FF)<<16U 
		| (uint(val.y)&0x000000FF)<<8U | (uint(val.x)&0x000000FF);
}

