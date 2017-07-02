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
uniform int maxStepSpecular;
uniform int maxStepDiffuse;
uniform float specularLOD;
uniform float diffuseLOD;

uniform sampler3D voxelTexture;
uniform sampler2D image;
uniform sampler2D gPositionRoughness;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuse;

vec3 indirectDiffuse(vec3 position, vec3 normal, vec3 albedo, vec3 kd, out float occlusion);
vec3 indirectSpecular(vec3 position, vec3 direction, vec3 normal, float roughness, vec3 ks);
vec3 indirectReflection(vec3 position, vec3 view, vec3 normal, float roughness, vec3 kd);

vec4 traceIndirectDiffuse(vec3 position, vec3 direction, float spread);
vec4 traceIndirectSpecular(vec3 position, vec3 direction, vec3 normal, float roughness);

vec4 sampleTexture(vec3 position, vec3 direction, float levelGain);
vec4 getFragmentColor(vec3 position, float lvl);

vec3 getClosestAxisNormal(vec3 v);
float getNodeBorderDistance(vec3 position, vec3 direction, int stepSize);
vec3 orthogonal(vec3 v);

vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness);
float doto(vec3 a, vec3 b);

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

	vec4 albedo = texture(gDiffuse, TexCoords);
	float roughness = posRough.w;
	float metallic = normMet.w;

	vec3 ks = calculateFresnelTerm(doto(normal, view), albedo.rgb, metallic, roughness);
	vec3 kd = 1.f - ks;

	float occlusion = 0.f;
	vec3 indirectDiff = indirectDiffuse(position, normal, albedo.rgb, kd, occlusion);
	vec3 indirectSpec = indirectSpecular(position, refl, normal, roughness, ks);
	vec3 solidColor = baseColor + indirectDiff + indirectSpec;
	//solidColor *= occlusion;

	if(albedo.a < 1.f) {
		vec3 refractionColor = indirectReflection(position, view, normal, roughness, kd);
		color = vec4(mix(refractionColor, solidColor, albedo.a), 1.f);
	}
	else
		color = vec4(solidColor, 1.f);
}


vec3 indirectDiffuse(vec3 position, vec3 normal, vec3 albedo, vec3 kd, out float occlusion) {
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

	vec4 color = vec4(0.f);
	color += weights[0] * traceIndirectDiffuse(startPos, sampleVectors[0], spreads[0]) * dot(sampleVectors[0], normal);

	for(int i = 1; i < 6; i++)
		color += weights[1] * traceIndirectDiffuse(startPos, sampleVectors[i], spreads[1]) * dot(sampleVectors[i], normal);

	color.rgb /= color.a;
	occlusion = 1.f - color.a / 2.f;

	return color.rgb * albedo * kd;// / PI;
}


vec3 indirectSpecular(vec3 position, vec3 direction, vec3 normal, float roughness, vec3 ks) {
	vec4 radiance = traceIndirectSpecular(position, direction, normal, roughness);

	return radiance.rgb * ks * dot(direction, normal);
}

vec3 indirectReflection(vec3 position, vec3 view, vec3 normal, float roughness, vec3 kd) {
	float refIndex = dot(kd, luminance);
	vec3 refrDir = refract(-view, normal, refIndex);
	vec4 radiance = traceIndirectSpecular(position, refrDir, normal, roughness);

	return radiance.rgb;
}

vec4 traceIndirectSpecular(vec3 position, vec3 direction, vec3 normal, float roughness) {
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

	int dynamicStep = int((1.f - roughness * roughness) * float(maxStepSpecular));
	while((color.a < 1.f) && counter < dynamicStep) {
		pos = startPos + dist * direction;
		float rest = 1.f - color.a;
		color += getFragmentColor(pos, lvl) * rest;

		lvl = roughness * log2(1 + dist * 0.33f);
		dist += pow(2.f, lvl) * THREEHALFS * specularLOD;

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
	while((color.a < 1.f) && counter < maxStepDiffuse) {
		pos = position + dist * direction;
		float rest = 1.f - color.a;
		color += getFragmentColor(pos, 0.8f + lvl) * rest;

		lvl = log2(1.f + dist * spread);
		dist += pow(2.f, lvl) * THREEHALFS * diffuseLOD;

		counter++;
	}

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

//Compute fresnel term with Fresnel-Schlick approximation
vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness) {
	vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

	//Simplified term withouth roughness included
    //return F0 + (1.0f - F0) * pow(1.0f - theta, 5.0f);
	vec3 fres = F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - theta, 5.0f);
	return clamp(fres, 0.f, 1.f);
}

float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0f);
}


