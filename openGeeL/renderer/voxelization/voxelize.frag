#version 430


in vec4 fragPosition;
in vec3 normal;
in vec2 texCoords;
in mat3 TBN;

flat in int  axis;
flat in vec4 AABB;

layout(location = 0) out vec4 gl_FragColor;
///*layout(pixel_center_integer)*/ in vec4 gl_FragCoord;

//Voxel data is stored in these 1D textures to be later integrated into concrete data structures
uniform layout(binding = 0, rgb10_a2ui) uimageBuffer voxelPositions;
uniform layout(binding = 1, rgba8) imageBuffer voxelColors;
uniform layout(binding = 2, rgba16f) imageBuffer voxelNormals;

//Atomic counter that is used to index above 1D textures
layout (binding = 0, offset = 0) uniform atomic_uint voxelCount;

uniform vec2 resolution;
uniform bool drawVoxel;

vec3 getIrradiance();

vec4 getFragCoords() {
	vec4 fragCoords;
	fragCoords.xy = (0.5f + (0.5f * fragPosition.xy / fragPosition.w)) * resolution;
	fragCoords.z = 0.5f + (0.5f * fragPosition.z / fragPosition.w);
	fragCoords.w = 1.f / fragPosition.w;

	return fragCoords;
}

uvec4 voxelizeSimple();
uvec4 voxelizeConservative();


//Mesh voxelization according to
//https://developer.nvidia.com/content/basics-gpu-voxelization and
//https://github.com/otaku690/SparseVoxelOctree
void main() {
	uvec4 coords = voxelizeSimple();
	//uvec4 coords = voxelizeConservative();

	uint index = atomicCounterIncrement(voxelCount);
	if(!drawVoxel) return; //Return in this case since we only want to count voxels

	//Example implementation. Should be later replaced with proper shading
	vec3 color = getIrradiance();

	imageStore(voxelPositions, int(index), coords);
	imageStore(voxelNormals, int(index), vec4(normal, 0.f));
	imageStore(voxelColors, int(index), vec4(color, 0.f));
}


uvec4 voxelizeSimple() {
	return uvec4(fragPosition);
}

uvec4 voxelizeConservative() {
	//Discard if fragment is outside of triangles bounding box
	discard(fragPosition.x < AABB.x || fragPosition.y < AABB.y 
		|| fragPosition.x > AABB.z || fragPosition.y > AABB.w);

	vec4 fragCoords = getFragCoords();
	uvec4 tCoords = uvec4(fragCoords.x, fragCoords.y, resolution.x * fragCoords.z, 0.f);
	uvec4 coords;
	if(axis == 1) {
	    coords.x = int(resolution.x) - tCoords.z;
		coords.z = tCoords.x;
		coords.y = tCoords.y;
	}
	else if(axis == 2) {
	    coords.z = tCoords.y;
		coords.y = int(resolution.x) - tCoords.z;
		coords.x = tCoords.x;
	}
	else
	    coords = tCoords;

	return coords;
}


//Cook-Torrance lighting............................................................................................................

const float PI = 3.14159265359;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D metal;

	int mapFlags;
	int type; //0 = Opaque, 1 = Cutout, 2 = Transparent
	float roughness;
	float metallic;
	vec3  color;
};

struct PointLight {
	samplerCube shadowMap;

    vec3 position;
    vec3 diffuse;

	float bias;
	float farPlane;

	int resolution;
	float scale;
	int type; //0: No 1: Hard 2: Soft shadow
};

struct SpotLight {
	sampler2D shadowMap;
	sampler2D cookie;
	mat4 lightTransform;

    vec3 position;
    vec3 direction;
	vec3 diffuse;

    float angle;
    float outerAngle;
	float bias;

	int resolution;
	float scale;
	int type; //0: No 1: Hard 2: Soft shadow
	bool useCookie;
};

const int DIRECTIONAL_SHADOWMAP_COUNT = 4;

struct DirectionalLight {
	sampler2D shadowMap;
	float cascadeEndClip[DIRECTIONAL_SHADOWMAP_COUNT];
	mat4 lightTransforms[DIRECTIONAL_SHADOWMAP_COUNT];

	vec3 direction;
    vec3 diffuse;

	float bias;
	int type; //0: No 1: Hard 2: Soft shadow
};

uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

uniform Material material;
uniform vec3 cameraPosition;

//Return dot(a,b) >= 0
float doto(vec3 a, vec3 b);

void readMaterialProperties(out vec3 albedo, out vec3 norm, out float roughness, out float metallic);

vec3 calculateReflectance(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic);

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness);


vec3 getIrradiance() {
	
	vec3 albedo, norm;
	float roughness, metallic;
	readMaterialProperties(albedo, norm, roughness, metallic);
	
	vec3 viewDirection = normalize(cameraPosition - fragPosition.xyz);

	vec3 irradiance = vec3(0.f);
	for(int i = 0; i < plCount; i++) 
		irradiance += calculatePointLight(i, pointLights[i], normal, fragPosition.xyz, viewDirection, albedo, roughness, metallic);
       
	for(int i = 0; i < dlCount; i++) 
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition.xyz, viewDirection, albedo, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition.xyz, viewDirection, albedo, roughness, metallic);

	irradiance = vec3(1.f) - exp(-irradiance * 2.2f); // Tone mapping

	return albedo * 0.05f;
	//return irradiance;
}


void readMaterialProperties(out vec3 albedo, out vec3 norm, out float roughness, out float metallic) {
	
	//Check if materials is actually textured
	float diffFlag = mod(material.mapFlags, 10);
	float specFlag = mod(material.mapFlags / 10, 10);
	float normFlag = mod(material.mapFlags / 100, 10);
	float metaFlag = mod(material.mapFlags / 1000, 10);

	//Discard fragment when material type is cutout and alpha value is very low
	discard(material.type == 1 && texture(material.diffuse, texCoords).a < 0.1f);

	norm = normalize(normal);
	if(normFlag == 1) {
		norm = texture(material.normal, texCoords).rgb;
		norm = normalize(norm * 2.f - 1.f);
		norm = normalize(TBN * norm);
	}

	albedo = (diffFlag == 1) ? texture(material.diffuse, texCoords).rgb : material.color;
	roughness = (specFlag == 1) ? 1.f - texture(material.specular, texCoords).r : material.roughness; 
	metallic = (metaFlag == 1) ? texture(material.metal, texCoords).r : material.metallic;
}


//Lighting.....................................................................................................................................

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
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic) {
	
	vec3 dir = lightPosition - fragPosition;
	vec3 lightDirection   = normalize(dir);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float NdotL = doto(normal, lightDirection);     

	float lightDistance = length(dir);
	float attenuation = 1.0f / (lightDistance * lightDistance); //Inverse square law
	vec3  radiance = lightDiffuse * attenuation;

	//BRDF
	float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(halfwayDirection, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic;

	//Lighting equation
	vec3  nom   = ndf * geo * ks; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	float denom =  4.0f * doto(viewDirection, normal) * NdotL + 0.001f; 
	vec3  brdf  = nom / denom;

	return ((kd * albedo / PI + brdf) * radiance) * NdotL; 
}

//Reflectance equation with Cook-Torrance BRDF for directional light
vec3 calculateReflectanceDirectional(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic) {
	
	vec3 dir = -lightPosition;
	vec3 lightDirection   = normalize(dir);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float NdotL = doto(normal, lightDirection);     

	vec3  radiance = lightDiffuse;

	//BRDF
	float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(halfwayDirection, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic;

	//Lighting equation
	vec3  nom   = ndf * geo * fres; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	float denom =  4.0f * doto(viewDirection, normal) * NdotL + 0.001f; 
	vec3  brdf  = nom / denom;

	return ((kd * albedo / PI + brdf) * radiance) * NdotL; 
}

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo, roughness, metallic);
	float shadow = 1.f;// - calculatePointLightShadows(index, normal, fragPosition);
	
    return shadow * reflectance;
}

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {
	
	vec3 lightDirection = normalize(light.position - fragPosition);

	//Set intensity depending on if object is inside spotlights halo (or outer halo)
	float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.angle - light.outerAngle);
    float intensity = clamp((theta - light.outerAngle) / epsilon, 0.f, 1.f);

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo, roughness, metallic);

	vec3 coords = vec3(0.f);
	float shadow = 1.f;// - calculateSpotLightShadows(index, normal, fragPosition, coords);
	float cookie = 1.f;//light.useCookie ? texture(light.cookie, coords.xy).r : 1.f;

    return shadow * reflectance * intensity * cookie;
}

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {
	
	vec3 reflectance = calculateReflectanceDirectional(fragPosition, normal, 
		viewDirection, light.direction, light.diffuse, albedo, roughness, metallic);
	float shadow = 1.f;// - calculateDirectionalLightShadows(index, normal, fragPosition);
	
    return shadow * reflectance;
}


//Helper functions......................................................................................................................

float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0f);
}

