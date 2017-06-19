#version 430

const float PI = 3.14159265359;
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
uniform int minStep;

uniform layout (binding = 0, r32ui) uimageBuffer nodeIndicies;
uniform layout (binding = 1, r32ui) uimageBuffer nodeDiffuse;

uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuseSpec;


vec4 getFragmentColor(vec3 position, int lvl);

//Traverse octree to obtain sample node and return if nonempty node was hit
bool sampleOctree(vec3 position, int maxLevel, out int nodeIndex);
//Traverse octree to obtain sample node and return reached level
bool sampleOctreeLvl(vec3 position, int maxLevel, out int nodeIndex, out int lvl);

vec4 raymarchOctree(vec3 position, vec3 direction, vec3 camPosition);

vec3 calculateReflectance(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightDirection, vec3 radiance, vec3 albedo, float roughness, float metallic);


vec4 linearFilter(vec3 position, int lvl, int nodeIndex);
vec4 linearMipmap(vec3 position, float level);
vec4 linearMipmapAndFilter(vec3 position, float level, int nodeIndex);

vec4 getColor(int nodeIndex);
float getLevel(float dist);
int getLevelDimension(int lvl);

float doto(vec3 a, vec3 b);
vec4 convRGBA8toVec4(uint val);
float planeIntersection(vec3 rayO, vec3 rayD, vec3 planeO, vec3 planeN);
vec4 mix2D(vec4 p00, vec4 p10, vec4 p01, vec4 p11, float x, float y);
vec4 mix3D(vec4 p000, vec4 p100, vec4 p010, vec4 p110, vec4 p001, vec4 p101, vec4 p011, vec4 p111, vec3 weight);


void main() {
	vec4 normMet  = texture(gNormalMet, TexCoords);
	vec4 diffSpec = texture(gDiffuseSpec, TexCoords);

	vec3 baseColor = texture(image, TexCoords).rgb;
	vec3 posView  = texture(gPositionDepth, TexCoords).rgb;
	vec3 normView = normalize(normMet.rgb);
	vec3 viewView = normalize(-posView);

	vec3 position = (inverseView * vec4(posView, 1.f)).xyz;
	vec3 normal = normalize((inverseView * vec4(origin + normView, 1.f)).xyz);
	vec3 view = normalize((inverseView * vec4(origin + viewView, 1.f)).xyz);
	vec3 refl = normalize(reflect(-view, normal));
	vec3 camPosition =(inverseView * vec4(vec3(0.f), 1.f)).xyz;

	vec3 albedo = diffSpec.rgb;
	float roughness = diffSpec.w;
	float metallic = normMet.w;
	float luma = dot(luminance, baseColor);

	vec4 radiance = raymarchOctree(position, refl, camPosition);
	vec3 irradiance = calculateReflectance(position, normal, view, refl, radiance.rgb, albedo, roughness, metallic);
	color = vec4(radiance.rgb, 1.f);
	//color = vec4(baseColor + luma * irradiance, 1.f);
	//color = getFragmentColor(position, level - 1);
}



vec4 raymarchOctree(vec3 position, vec3 direction, vec3 camPosition) {
	
	bool hit = false;
	int depth = 0;
	int counter = 0;
	int nodeIndex;
	float lvl = level;
	vec3 pos = position;

	int minStepSize = getLevelDimension(int(lvl + 1));
	float minDist = float(getLevelDimension(int(lvl)));
	
	//float viewDist = length(position - camPosition);
	//float newLVL = getLevel(viewDist);
	//float minDist = float(getLevelDimension(int(newLVL)));

	float dist = minDist;
	
	while(!hit && counter < minStep) {

		pos += minDist * direction;
		hit = sampleOctreeLvl(pos, int(lvl), nodeIndex, depth);

		int stepSize = getLevelDimension(depth + 1);
		ivec3 posFloor = (ivec3(pos) / stepSize) * stepSize;
		ivec3 posCeil  = ((ivec3(pos) + stepSize) / stepSize) * stepSize; 

		float distances[6];
		distances[0] = planeIntersection(pos, direction, vec3(posFloor), vec3(1.f, 0.f, 0.f));
		distances[1] = planeIntersection(pos, direction, vec3(posFloor), vec3(0.f, 1.f, 0.f));
		distances[2] = planeIntersection(pos, direction, vec3(posFloor), vec3(0.f, 0.f, 1.f));
		distances[3] = planeIntersection(pos, direction, vec3(posCeil),  vec3(1.f, 0.f, 0.f));
		distances[4] = planeIntersection(pos, direction, vec3(posCeil),  vec3(0.f, 1.f, 0.f));
		distances[5] = planeIntersection(pos, direction, vec3(posCeil),  vec3(0.f, 0.f, 1.f));

		minDist = FLOAT_MAX;
		for(int i = 0; i < 6; i++)
			minDist = min(minDist, distances[i]);

		//Add little offset to avoid placement into same tree node
		minDist += 0.1f;
		dist += minDist;

		//lvl = log(dimensions / dist) / log(2) + 1.f;

		counter++;
	}

	//if(!hit)
	//	return getFragmentColor(position, level);

	//return linearFilter(pos, int(lvl), nodeIndex);
	return getColor(nodeIndex);
}


vec4 getFragmentColor(vec3 position, int lvl) {
	int nodeIndex;
	sampleOctree(position, lvl, nodeIndex);

	return getColor(nodeIndex);
}

bool sampleOctree(vec3 position, int maxLevel, out int nodeIndex) {
	int lvl;
	return sampleOctreeLvl(position, maxLevel, nodeIndex, lvl);
}

bool sampleOctreeLvl(vec3 position, int maxLevel, out int nodeIndex, out int lvl) {
	
	//Dimensions of root node
	unsigned int dim = dimensions;
	uvec3 umin = uvec3(0);

	nodeIndex = 0; //Set index to root node (0)
	unsigned int node = imageLoad(nodeIndicies, nodeIndex).r;

	//Iterate through the levels and find suitable child node for voxel 
	for(int i = 0; i < maxLevel; i++) {
		if((node & 0x80000000) == 0) {
			lvl = i;
			return false;
		}

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

	lvl = maxLevel;
	return (node & 0x80000000) != 0;
}



//Lighting.................................................................................................................................

//Compute fresnel term with Fresnel-Schlick approximation
vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness) {
	vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

	//Simplified term withouth roughness included
    //return F0 + (1.0f - F0) * pow(1.0f - theta, 5.0f);
	vec3 fres = F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - theta, 5.0f);
	return clamp(fres, 0.f, 1.f);
}

//Trowbridge-Reitz GGX normal distribution function
float calculateNormalDistrubution(vec3 normal, vec3 halfway, float roughness) {
    float a = roughness * roughness;
    float NdotH  = doto(normal, halfway);
    float NdotH2 = NdotH * NdotH;
	
    float denom  = (NdotH2 * (a - 1.0f) + 1.0f);
    denom = PI * denom * denom;
    return a / denom;
}

float calculateGeometryFunctionSchlick(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float nom   = NdotV;
    float denom = NdotV * (1.0f - k) + k;
	
    return nom / denom;
}

float calculateGeometryFunctionSmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness) {
    float NdotV = doto(normal, viewDirection);
    float NdotL = doto(normal, lightDirection);
    float ggx2  = calculateGeometryFunctionSchlick(NdotV, roughness);
    float ggx1  = calculateGeometryFunctionSchlick(NdotL, roughness);
	
    return ggx1 * ggx2;
}

//Reflectance equation with Cook-Torrance BRDF
vec3 calculateReflectance(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightDirection, vec3 radiance, vec3 albedo, float roughness, float metallic) {
	
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float NdotL = doto(normal, lightDirection);     

	//BRDF
	float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(halfwayDirection, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.f) - ks;
    kd *= 1.f - metallic;

	//Lighting equation
	vec3  nom   = ndf * geo * ks; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	float denom =  4.f * doto(viewDirection, normal) * NdotL + 0.001f; 
	vec3  brdf  = nom / denom;

	return brdf * radiance * NdotL; 
	//return ((kd * albedo / PI + brdf) * radiance) * NdotL; 
}


//Helper functions......................................................................................................................


vec4 linearFilter(vec3 position, int lvl, int fallBack) {
	int dim = getLevelDimension(lvl);
	ivec3 posFloor = (ivec3(position) / dim) * dim;
	ivec3 posCeil  = ((ivec3(position) + dim) / dim) * dim; 

	vec3 posFrag = position - posFloor;
	posFrag /= dim;

	int samples[8];
	bool hits[8];
	hits[0] = sampleOctree(vec3(posFloor), lvl, samples[0]);
	hits[1] = sampleOctree(vec3(posCeil.x, posFloor.y, posFloor.z), lvl, samples[1]);
	hits[2] = sampleOctree(vec3(posFloor.x, posCeil.y, posFloor.z), lvl, samples[2]);
	hits[3] = sampleOctree(vec3(posCeil.x, posCeil.y, posFloor.z), lvl, samples[3]);
	hits[4] = sampleOctree(vec3(posFloor.x, posFloor.y, posCeil.z), lvl, samples[4]);
	hits[5] = sampleOctree(vec3(posCeil.x, posFloor.y, posCeil.z), lvl, samples[5]);
	hits[6] = sampleOctree(vec3(posFloor.x, posCeil.y, posCeil.z), lvl, samples[6]);
	hits[7] = sampleOctree(vec3(posCeil), lvl, samples[7]);

	vec4 p[8];
	for(int i = 0; i < 8; i++) {
		float hit = float(hits[i]);
		p[i] = hit * getColor(samples[i]) + (1.f - hit) * getColor(fallBack);
	}
		

	return mix3D(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], posFrag); 
}

vec4 linearMipmap(vec3 position, float level) {
	int floorLevel = int(floor(level));
	float fragLevel = level - float(floorLevel);

	vec4 a = getFragmentColor(position, floorLevel);
	vec4 b = getFragmentColor(position, int(ceil(level)));

	return mix(a, b, fragLevel); 
}

vec4 linearMipmapAndFilter(vec3 position, float level, int fallBack) {
	int floorLevel = int(floor(level));
	float fragLevel = level - float(floorLevel);

	vec4 a = linearFilter(position, floorLevel, fallBack);
	vec4 b = linearFilter(position, int(ceil(level)), fallBack);

	return mix(a, b, fragLevel); 
}


vec4 getColor(int nodeIndex) {
	unsigned int rgb8 = imageLoad(nodeDiffuse, nodeIndex).r;
	return convRGBA8toVec4(rgb8).rgba / 255;
}

float getLevel(float dist) {
	float d = abs(dist) / farClip; //Clamp in clip range
	d = pow(d, 0.8f);

	float lvl = float(level);
	return lvl - d * lvl;
}

int getLevelDimension(int lvl) {
	return dimensions / int(pow(2, lvl - 1));
}

float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0f);
}

vec4 convRGBA8toVec4(uint val) {
	return vec4(float((val&0x000000FF)), float((val&0x0000FF00)>>8U),
				float((val&0x00FF0000)>>16U), float((val&0xFF000000)>>24U));
}

float planeIntersection(vec3 rayO, vec3 rayD, vec3 planeO, vec3 planeN) {
	float v = dot(planeN, rayD) + epsilon;
	vec3 a = planeO - rayO;
	float d = dot(a, planeN) / v;

	//float pick = step(d, 0.f);
	//return (pick * d) + ((1.f - pick) * FLOAT_MAX);

	if(d > epsilon)
		return d;

	return FLOAT_MAX;
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