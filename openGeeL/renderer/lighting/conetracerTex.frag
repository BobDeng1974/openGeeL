#version 430

const float PI = 3.14159265359;
const float epsilon = 0.000001f;
const float VOXELSIZE = 1.f;
const float FLOAT_MAX = 100000.f;
const float THREEHALFS = 3.f / 2.f;
const vec3  luminance = vec3(0.299f, 0.587f, 0.114f);

in vec2 TexCoords;

layout (location = 0) out vec4 color;

uniform mat4 inverseView;
uniform vec3 origin;

uniform int dimensions;
uniform float farClip;
uniform int minStep;

uniform sampler3D voxelTexture;
uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuseSpec;

vec3 indirectDiffuse(vec3 position, vec3 normal, vec3 albedo);
vec3 indirectSpecular(vec3 position, vec3 direction, vec3 normal, float roughness);

vec4 traceIndirectDiffuse(vec3 position, vec3 direction, float spread);
vec4 traceIndirectSpecular(vec3 position, vec3 direction, vec3 normal, float specularity);

vec4 sampleTexture(vec3 position, vec3 direction, float levelGain);
vec4 getFragmentColor(vec3 position, float lvl);

vec3 getClosestAxisNormal(vec3 v);
float getNodeBorderDistance(vec3 position, vec3 direction, int stepSize);
vec3 orthogonal(vec3 v);


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
	vec3 camPosition = (inverseView * vec4(vec3(0.f), 1.f)).xyz;

	vec3 albedo = diffSpec.rgb;
	float roughness = diffSpec.w;
	float metallic = normMet.w;

	vec3 indirectDiffuse = indirectDiffuse(position, normal, albedo);
	vec3 indirectSpecular = indirectSpecular(position, refl, normal, roughness);

	color = vec4(baseColor + indirectDiffuse + indirectSpecular, 1.f);
}




vec3 indirectDiffuse(vec3 position, vec3 normal, vec3 albedo) {
	//Move position into direction of voxel border to avoid 
	//sampling neighboring voxels when direction is steep
	vec3 halfDir = getClosestAxisNormal(normal);
	float offset = getNodeBorderDistance(position, halfDir, int(VOXELSIZE * 16.f));
	offset = offset / dot(normal, halfDir); //Projection of border dist onto direction vector
	vec3 startPos = position + offset * halfDir;

	vec3 right = normalize(orthogonal(normal));
	vec3 forward = normalize(cross(normal, right));
	
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

	float spreads[2];
	spreads[0] = 0.33f;
	spreads[1] = 0.166f;

	vec3 color = vec3(0.f);
	color += weights[0] * traceIndirectDiffuse(startPos, sampleVectors[0], spreads[0]).rgb;
	color += weights[1] * traceIndirectDiffuse(startPos, sampleVectors[1], spreads[1]).rgb;
	color += weights[1] * traceIndirectDiffuse(startPos, sampleVectors[2], spreads[1]).rgb;
	color += weights[1] * traceIndirectDiffuse(startPos, sampleVectors[3], spreads[1]).rgb;
	color += weights[1] * traceIndirectDiffuse(startPos, sampleVectors[4], spreads[1]).rgb;
	color += weights[1] * traceIndirectDiffuse(startPos, sampleVectors[5], spreads[1]).rgb;

	return color * albedo;
}


vec3 indirectSpecular(vec3 position, vec3 direction, vec3 normal, float roughness) {
	float specularity = (1.f - roughness);
	vec4 radiance = traceIndirectSpecular(position, direction, normal, specularity);

	return radiance.rgb * specularity;
}

vec4 traceIndirectSpecular(vec3 position, vec3 direction, vec3 normal, float specularity) {
	//Move position into direction of voxel border to avoid 
	//sampling neighboring voxels when direction is steep
	vec3 halfDir = getClosestAxisNormal(normal);
	float borderDist = getNodeBorderDistance(position, halfDir, int(VOXELSIZE * 32.f));
	vec3 startPos = position + borderDist * halfDir;

	float dist = 0.001f;
	vec3 pos = vec3(0.f);
	float lvl = 0.f;
	int counter = 0;
	vec4 color = vec4(0.f);
	while((color.a < 1.f) && counter < minStep) {
		pos = startPos + dist * direction;
		float rest = 1.f - color.a;
		color += getFragmentColor(pos, lvl) * rest;

		lvl = specularity * log2(1 + dist * 0.33f);
		dist += pow(2.f, lvl) * THREEHALFS;

		counter++;
	}

	color /= color.a;
	color = clamp(color, 0.f, 1.f);

	return color;
}

vec4 traceIndirectDiffuse(vec3 position, vec3 direction, float spread) {
	vec3 pos = position;

	float dist = 0.1f;
	float lvl = 0.f;
	int counter = 0;
	vec4 color = vec4(0.f);
	while((color.a < 1.f) && counter < minStep) {
		pos = position + dist * direction;
		float rest = 1.f - color.a;
		color += getFragmentColor(pos, 0.8f + lvl) * rest;

		lvl = log2(1.f + dist * spread);
		dist += pow(2.f, lvl) * THREEHALFS;

		counter++;
	}

	color /= color.a;
	color = clamp(color, 0.f, 1.f);

	return color;
}


vec4 getFragmentColor(vec3 position, float lvl) {
	vec4 col = textureLod(voxelTexture, position * 0.5f / dimensions, lvl); 
	col.rgb = pow(col.rgb, vec3(2.2f)); //Gamma correction

	return col;
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


float planeIntersection(vec3 rayO, vec3 rayD, vec3 planeO, vec3 planeN) {
	float v = dot(planeN, rayD) + epsilon;
	vec3 a = planeO - rayO;
	float d = dot(a, planeN) / v;

	float pick = step(epsilon, d);
	return (pick * d) + ((1.f - pick) * FLOAT_MAX);
}


float getNodeBorderDistance(vec3 position, vec3 direction, int stepSize) {
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


vec3 orthogonal(vec3 v) {
	v = normalize(v);
	vec3 o = vec3(1.f, 0.f, 0.f);

	return abs(dot(v, o)) > 0.999f ? cross(v, vec3(0.f, 1.f, 0.f)) : cross(v, o);
}


