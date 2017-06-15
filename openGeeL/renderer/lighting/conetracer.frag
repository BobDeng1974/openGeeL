#version 430

const float PI = 3.14159265359;

in vec2 TexCoords;

layout (location = 0) out vec4 color;

uniform mat4 inverseView;
uniform vec3 origin;

uniform int level;
uniform int dimensions;

uniform layout (binding = 0, r32ui) uimageBuffer nodeIndicies;
uniform layout (binding = 1, r32ui) uimageBuffer nodeDiffuse;

uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;


//Return dot(a,b) >= 0
float doto(vec3 a, vec3 b);
vec4 convRGBA8toVec4(uint val);

//Traverse octree and obtain sample node
bool sampleOctree(vec3 position, int maxLevel, out int nodeIndex);

void main() {
	
	vec3 baseColor = texture(image, TexCoords).rgb;
	vec3 posView  = texture(gPositionDepth, TexCoords).rgb;
	vec3 normView = texture(gNormalMet, TexCoords).rgb;
	vec3 viewView = normalize(-posView);

	vec3 position = (inverseView * vec4(posView, 1.f)).xyz;
	vec3 normal = normalize((inverseView * vec4(origin + normView, 1.f)).xyz);
	vec3 view = normalize((inverseView * vec4(origin + viewView, 1.f)).xyz);

	//float dist = -posView.z / 100.f;
	//dist = dist * dist;
	//dist = 1.f - dist;
	int lvl = level - 1;//1 + int(dist * float(level - 1));

	int nodeIndex;
	bool hit = sampleOctree(position, lvl, nodeIndex);
	
	if(hit) {
		unsigned int rgb8 = imageLoad(nodeDiffuse, nodeIndex).r;
		vec4 diffuse = convRGBA8toVec4(rgb8).rgba / 255;

		color = diffuse;
	}
	else
		color = vec4(vec3(0.5f), 1.f);

	//Visualize basic scene on left side
	if(TexCoords.x < 0.4f)
		color = vec4(baseColor, 1.f);
}


bool sampleOctree(vec3 position, int maxLevel, out int nodeIndex) {
	
	//Dimensions of root node
	unsigned int dim = dimensions;
	uvec3 umin = uvec3(0, 0, 0);

	nodeIndex = 0; //Set index to root node (0)
	unsigned int node = imageLoad(nodeIndicies, nodeIndex).r;

	//Iterate through the levels and find suitable child node for voxel 
	for(int i = 0; i < maxLevel; i++) {
		if((node & 0x80000000) == 0)
			return false;

		nodeIndex = int(node & 0x7FFFFFFF);
		
		dim /= 2;
		//Spacial index of subnode
		uvec3 box = clamp(ivec3(1 + position - umin - dim), 0, 1);
		umin += box * dim;

		//Spacial position translated into index
		unsigned int childIndex = box.x + 4 * box.y + 2 * box.z;
		nodeIndex += int(childIndex);

		node = imageLoad(nodeIndicies, nodeIndex).r;
	}

	return true;
}


//Helper functions......................................................................................................................

float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0f);
}

vec4 convRGBA8toVec4(uint val) {
	return vec4(float((val&0x000000FF)), float((val&0x0000FF00)>>8U),
				float((val&0x00FF0000)>>16U), float((val&0xFF000000)>>24U));
}
