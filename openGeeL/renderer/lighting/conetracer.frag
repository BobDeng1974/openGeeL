#version 430

#include <renderer/shaders/helperfunctions.glsl>

const float epsilon = 0.000001f;
const float FLOAT_MAX = 100000.f;
const vec3  luminance = vec3(0.299f, 0.587f, 0.114f);

in vec2 TexCoords;

layout (location = 0) out vec4 color;

uniform mat4 inverseView;
uniform vec3 origin;

uniform int level;
uniform int dimensions;
uniform float farClip;
uniform int maxStepSpecular;
uniform int maxStepDiffuse;

uniform layout (binding = 0, r32ui) uimageBuffer nodeIndicies;
uniform layout (binding = 1, r32ui) uimageBuffer nodeDiffuse;

uniform sampler2D image;
uniform sampler2D gPositionRoughness;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuse;


vec3 indirectDiffuse(vec3 position, vec3 normal, vec3 albedo);
vec3 indirectSpecular(vec3 position, vec3 direction, vec3 normal, float roughness);

vec4 getFragmentColor(vec3 position, int lvl);
vec4 raymarchOctree(vec3 position, vec3 direction, vec3 camPosition, int maxLevel);
vec4 raymarchOctreeMirror(vec3 position, vec3 direction, vec3 normal, int maxLevel);

//Traverse octree to obtain sample node and return reached level
bool sampleOctreeNearest(vec3 position, int maxLevel, out vec4 col, out int depth);
bool sampleOctreeLinear(vec3 position, int maxLevel, out vec4 col, out int depth);
bool sampleOctreeMipmap(vec3 position, float lvl, out vec4 col, out int depth);
bool sampleOctreeLinearMipmap(vec3 position, float lvl, out vec4 col, out int depth);


vec4 getColorNearest(int nodeIndex);
vec4 getColorLinear(int parentIndex, vec3 factor);
vec4 getColorMipmap(int index1, int index2, float factor);
vec4 getColorLinearMipmap(int index1, int index2, vec3 fac1, vec3 fac2, float level);

float getLevelDimensionFloat(float lvl);
int   getLevelDimension(int lvl);

float getNodeBorderDistance(vec3 position, vec3 direction, int depth);
float planeIntersection(vec3 rayO, vec3 rayD, vec3 planeO, vec3 planeN);

vec3 getClosestAxisNormal(vec3 v);
vec4 convRGBA8toVec4(uint val);
vec4 mix2D(vec4 p00, vec4 p10, vec4 p01, vec4 p11, float x, float y);
vec4 mix3D(vec4 p000, vec4 p100, vec4 p010, vec4 p110, vec4 p001, vec4 p101, vec4 p011, vec4 p111, vec3 weight);


void main() {
	vec4 normMet  = texture(gNormalMet, TexCoords);
	vec4 posRough = texture(gPositionRoughness, TexCoords);

	vec3 baseColor = texture(image, TexCoords).rgb;
	vec3 posView  = posRough.rgb;
	vec3 normView = normalize(normMet.rgb);
	vec3 viewView = normalize(-posView);

	vec3 position = (inverseView * vec4(posView, 1.f)).xyz;
	vec3 normal = normalize((inverseView * vec4(origin + normView, 1.f)).xyz);
	vec3 view = normalize((inverseView * vec4(origin + viewView, 1.f)).xyz);
	vec3 refl = normalize(reflect(-view, normal));
	vec3 camPosition = (inverseView * vec4(vec3(0.f), 1.f)).xyz;

	vec3 albedo = texture(gDiffuse, TexCoords).rgb;
	float roughness = posRough.a;
	float metallic = normMet.w;

	//vec3 indirectDiffuse = indirectDiffuse(position, normal, albedo);
	//vec3 indirectSpecular = indirectSpecular(position, refl, normal, roughness);
	vec3 indirectSpecular = raymarchOctreeMirror(position, refl, normal, level).rgb * (1.f - roughness);

	color = vec4(indirectSpecular, 1.f);
	//color = vec4(baseColor + indirectDiffuse + indirectSpecular, 1.f);
}


vec3 indirectDiffuse(vec3 position, vec3 normal, vec3 albedo) {
	vec3 right = cross(normal, orthogonal(normal));
	vec3 forward = cross(normal, right);
	
	vec3 sampleVectors[6];
	sampleVectors[0] = vec3(0.f, 1.f, 0.f);
	sampleVectors[1] = vec3(0.f, 0.5f, 0.866f);
	sampleVectors[2] = vec3(0.824f, 0.5f, 0.268f);
	sampleVectors[3] = vec3(0.51f, 0.5f, -0.7f);
	sampleVectors[4] = vec3(-0.51f, 0.5f, -0.7f);
	sampleVectors[5] = vec3(-0.824f, 0.5f, 0.268f);

	for(int i = 0; i < 6; i++) 
		sampleVectors[i] = forward * sampleVectors[i].x + normal * sampleVectors[i].y + right * sampleVectors[i].z;

	float weights[2];
	weights[0] = PI / 4.f;
	weights[1] = (3.f * PI) / 20.f;

	int lvl = level - 3;
	vec3 color = vec3(0.f);
	color += weights[0] * raymarchOctree(position, sampleVectors[0], normal, lvl).rgb;
	color += weights[1] * raymarchOctree(position, sampleVectors[1], normal, lvl).rgb;
	color += weights[1] * raymarchOctree(position, sampleVectors[2], normal, lvl).rgb;
	color += weights[1] * raymarchOctree(position, sampleVectors[3], normal, lvl).rgb;
	color += weights[1] * raymarchOctree(position, sampleVectors[4], normal, lvl).rgb;
	color += weights[1] * raymarchOctree(position, sampleVectors[5], normal, lvl).rgb;

	return color * albedo;
}

vec3 indirectSpecular(vec3 position, vec3 direction, vec3 normal, float roughness) {
	vec4 radiance = raymarchOctree(position, direction, normal, level);

	return radiance.rgb * (1.f - roughness);
}


vec4 getFragmentColor(vec3 position, int lvl) {
	int nodeIndex;
	vec4 col = vec4(0.f);
	sampleOctreeNearest(position, lvl, col, nodeIndex);

	return col;
}

vec4 raymarchOctree(vec3 position, vec3 direction, vec3 normal, int maxLevel) {
	bool hit = false;
	int depth = 0;
	int counter = 0;
	int nodeIndex;
	float lvl = maxLevel;

	//Move position into direction of voxel border to avoid 
	//sampling neighboring voxels when direction is steep
	vec3 halfDir = getClosestAxisNormal(normal);
	float borderDist = getNodeBorderDistance(position, halfDir, int(lvl - 2));
	
	vec3 pos = position + borderDist * halfDir;

	float minDist = 0.1f;
	float dist = 0.1f;
	
	vec4 color = vec4(0.f);
	while((color.a < 0.5f) && counter < 15) {
		pos += minDist * direction;
		vec4 currColor = vec4(0.f);
		hit = sampleOctreeMipmap(pos, lvl, currColor, depth);

		float rest = 1.f - color.a;
		color += currColor * rest;
		minDist = getNodeBorderDistance(pos, direction, depth);
		//minDist = getLevelDimensionFloat(lvl + 1) * (3.f / 2.f);

		dist += minDist;
		lvl = log(dimensions / (dist)) / log(2) + 1.f;

		counter++;
	}

	color /= color.a;
	color = clamp(color, 0.f, 1.f);

	return color;
}

vec4 raymarchOctreeMirror(vec3 position, vec3 direction, vec3 normal, int maxLevel) {
	bool hit = false;
	int depth = 0;
	int counter = 0;
	int nodeIndex;
	float lvl = maxLevel;

	//Move position into direction of voxel border to avoid 
	//sampling neighboring voxels when direction is steep
	vec3 halfDir = getClosestAxisNormal(normal);
	float borderDist = getNodeBorderDistance(position, halfDir, int(lvl));
	float borderDistProj = borderDist / dot(direction, halfDir); //Projection of border dist onto direction vector
	
	vec3 pos = position + borderDistProj * direction;
	float minDist = 0.f;
	
	vec4 color = vec4(0.f);
	while((color.a < 0.5f) && counter < 15) {
		pos += minDist * direction;

		vec4 currColor = vec4(0.f);
		//hit = sampleOctreeNearest(pos, int(floor(lvl)), currColor, depth);
		hit = sampleOctreeLinear(pos, int(floor(lvl)), currColor, depth);

		float rest = 1.f - color.a;
		color += currColor * rest;

		minDist = getNodeBorderDistance(pos, direction, depth);
		counter++;
	}

	color /= color.a;
	color = clamp(color, 0.f, 1.f);
	color += getFragmentColor(position, level) * float(!hit); //Add fallback color if nothing was hit

	return color;
}



bool sampleOctreeNearest(vec3 position, int maxLevel, out vec4 col, out int depth) {
	
	//Dimensions of root node
	unsigned int dim = dimensions;
	uvec3 umin = uvec3(0);

	int nodeIndex = 0;
	unsigned int node = imageLoad(nodeIndicies, nodeIndex).r;

	//Iterate through the levels and find suitable child node for voxel 
	for(int i = 0; i < maxLevel; i++) {
		nodeIndex = int(node & 0x7FFFFFFF);
		
		dim /= 2;
		//Spacial index of subnode
		vec3 frag = position - umin;
		uvec3 box = clamp(ivec3(1 + frag - dim), 0, 1);
		umin += box * dim;

		//Spacial position translated into index
		unsigned int childIndex = box.x + 4 * box.y + 2 * box.z;
		nodeIndex += int(childIndex);
		node = imageLoad(nodeIndicies, nodeIndex).r;

		if((node & 0x80000000) == 0) {
			col = getColorNearest(nodeIndex);

			depth = i + 1;
			return false;
		}
	}

	col = getColorNearest(nodeIndex);

	depth = maxLevel;
	return (node & 0x80000000) != 0;
}

bool sampleOctreeLinear(vec3 position, int maxLevel, out vec4 col, out int depth) {
	
	//Dimensions of root node
	unsigned int dim = dimensions;
	uvec3 umin = uvec3(0);
	vec3 frag = vec3(0.f);

	int parentIndex = 0;
	int nodeIndex = 0; //Set index to root node (0)
	unsigned int node = imageLoad(nodeIndicies, nodeIndex).r;

	//Iterate through the levels and find suitable child node for voxel 
	for(int i = 0; i < maxLevel; i++) {
		nodeIndex = int(node & 0x7FFFFFFF);
		
		frag = (position - umin) / dim;
		dim /= 2;
		//Spacial index of subnode
		uvec3 box = clamp(ivec3(1 + position - umin - dim), 0, 1);
		umin += box * dim;

		//Spacial position translated into index
		unsigned int childIndex = box.x + 4 * box.y + 2 * box.z;
		parentIndex = nodeIndex;
		nodeIndex += int(childIndex);
		node = imageLoad(nodeIndicies, nodeIndex).r;

		if((node & 0x80000000) == 0) {
			col = getColorLinear(parentIndex, frag);

			depth = i + 1;
			return false;
		}
	}

	col = getColorLinear(parentIndex, frag);

	depth = maxLevel;
	return (node & 0x80000000) != 0;
}

bool sampleOctreeMipmap(vec3 position, float lvl, out vec4 col, out int depth) {
	int maxLevel = int(ceil(lvl));

	//Dimensions of root node
	unsigned int dim = dimensions;
	uvec3 umin = uvec3(0);

	int parentIndex = 0;
	int nodeIndex = 0;
	unsigned int node = imageLoad(nodeIndicies, nodeIndex).r;

	//Iterate through the levels and find suitable child node for voxel 
	for(int i = 0; i < maxLevel; i++) {
		nodeIndex = int(node & 0x7FFFFFFF);
		
		dim /= 2;
		//Spacial index of subnode
		uvec3 box = clamp(ivec3(1 + position - umin - dim), 0, 1);
		umin += box * dim;

		//Spacial position translated into index
		unsigned int childIndex = box.x + 4 * box.y + 2 * box.z;
		parentIndex = nodeIndex;
		nodeIndex += int(childIndex);
		node = imageLoad(nodeIndicies, nodeIndex).r;

		if((node & 0x80000000) == 0) {
			col = getColorMipmap(parentIndex, nodeIndex, lvl);

			depth = i + 1;
			return false;
		}
	}

	col = getColorMipmap(parentIndex, nodeIndex, lvl);

	depth = maxLevel;
	return (node & 0x80000000) != 0;
}

bool sampleOctreeLinearMipmap(vec3 position, float lvl, out vec4 col, out int depth) {
	//TODO: implement this

	return false;
}



//Helper functions......................................................................................................................


float getNodeBorderDistance(vec3 position, vec3 direction, int depth) {
	int stepSize = getLevelDimension(depth + 1);
	ivec3 posFloor = (ivec3(position) / stepSize) * stepSize;
	ivec3 posCeil  = ((ivec3(position) + stepSize) / stepSize) * stepSize; 

	float distances[6];
	distances[0] = planeIntersection(position, direction, vec3(posFloor), vec3(1.f, 0.f, 0.f));
	distances[1] = planeIntersection(position, direction, vec3(posFloor), vec3(0.f, 1.f, 0.f));
	distances[2] = planeIntersection(position, direction, vec3(posFloor), vec3(0.f, 0.f, 1.f));
	distances[3] = planeIntersection(position, direction, vec3(posCeil),  vec3(1.f, 0.f, 0.f));
	distances[4] = planeIntersection(position, direction, vec3(posCeil),  vec3(0.f, 1.f, 0.f));
	distances[5] = planeIntersection(position, direction, vec3(posCeil),  vec3(0.f, 0.f, 1.f));

	float minDist = FLOAT_MAX;
	for(int i = 0; i < 6; i++)
		minDist = min(minDist, distances[i]);

	//Add little offset to avoid placement into same tree node
	minDist += 0.1f;

	return minDist;
}


float planeIntersection(vec3 rayO, vec3 rayD, vec3 planeO, vec3 planeN) {
	float v = dot(planeN, rayD) + epsilon;
	vec3 a = planeO - rayO;
	float d = dot(a, planeN) / v;

	float pick = step(epsilon, d);
	return (pick * d) + ((1.f - pick) * FLOAT_MAX);
}




vec4 getColorNearest(int nodeIndex) {
	unsigned int rgb8 = imageLoad(nodeDiffuse, nodeIndex).r;
	vec4 col = convRGBA8toVec4(rgb8).rgba / 255;
	col.rgb = pow(col.rgb, vec3(2.2f)); //Gamma correction

	return col;
}

vec4 getColorLinear(int parentIndex, vec3 factor) {
	uvec3 childBoxes[8];
	childBoxes[0] = uvec3(0, 0, 0);
	childBoxes[1] = uvec3(1, 0, 0);
	childBoxes[2] = uvec3(0, 1, 0);
	childBoxes[3] = uvec3(1, 1, 0);
	childBoxes[4] = uvec3(0, 0, 0);
	childBoxes[5] = uvec3(0, 0, 1);
	childBoxes[6] = uvec3(0, 1, 1);
	childBoxes[7] = uvec3(1, 1, 1);

	vec4 p[8]; //Colors of child nodes
	for(int i = 0; i < 8; i++) {
		unsigned int offset = childBoxes[i].x + 4 * childBoxes[i].y + 2 * childBoxes[i].z;  
		int index = parentIndex + int(offset);

		p[i] = getColorNearest(index);
	}

	return mix3D(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], factor); 
}

vec4 getColorMipmap(int index1, int index2, float level) {
	float factor = level - floor(level); 

	vec4 col1 = getColorNearest(index1);
	vec4 col2 = getColorNearest(index2);

	return mix(col1, col2, factor);
}

vec4 getColorLinearMipmap(int index1, int index2, vec3 fac1, vec3 fac2, float level) {
	float factor = level - floor(level); 

	vec4 col1 = getColorLinear(index1, fac1);
	vec4 col2 = getColorLinear(index2, fac2);

	return mix(col1, col2, factor);
}



int getLevelDimension(int lvl) {
	return dimensions / int(pow(2, lvl - 1));
}

float getLevelDimensionFloat(float lvl) {
	return float(dimensions) / pow(2.f, lvl - 1.f);
}

vec3 getClosestAxisNormal(vec3 v) {
	vec3 a = abs(v);
	
	float XgY = step(a.y, a.x);
	float XgZ = step(a.z, a.x);
	float YgZ = step(a.z, a.y);

	float maxX = XgY * XgZ;
	float maxY = ((1.f - XgY) * YgZ) * 2.f;
	float maxZ = ((1.f - XgZ) * (1.f - YgZ)) * 3.f;

	int maxAxis = int(max(maxX, max(maxY, maxZ))) - 1;

	vec3 normal = vec3(0.f);
	normal[maxAxis] = v[maxAxis] / a[maxAxis];

	return normal;
}

vec4 convRGBA8toVec4(uint val) {
	return vec4(float((val&0x000000FF)), float((val&0x0000FF00)>>8U),
				float((val&0x00FF0000)>>16U), float((val&0xFF000000)>>24U));
}

vec4 mix2D(vec4 p00, vec4 p10, vec4 p01, vec4 p11, float x, float y) {
	vec4 p0 = mix(p00, p10, x);
	vec4 p1 = mix(p01, p11, x);

	return mix(p0, p1, y);
}

vec4 mix3D(vec4 p000, vec4 p100, vec4 p010, vec4 p110, vec4 p001, vec4 p101, vec4 p011, vec4 p111, vec3 weight) {
	vec4 p0 = mix2D(p000, p100, p010, p110, weight.x, weight.y);
	vec4 p1 = mix2D(p001, p101, p011, p111, weight.x, weight.y);

	return mix(p0, p1, weight.z);
}