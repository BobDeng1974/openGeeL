#version 430

#include <shaders/helperfunctions.glsl>
#include <shaders/material.glsl>
#include <shaders/lighting/lights.glsl>
#include <shaders/lighting/cooktorrance.glsl>

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

	vec3 color = getIrradiance();

	imageStore(voxelPositions, int(index), coords);
	imageStore(voxelNormals, int(index), vec4(normal, 0.f));
	imageStore(voxelColors, int(index), vec4(color, 1.f));
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

uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

uniform Material material;
uniform vec3 cameraPosition;

#include <shaders/materialproperties.glsl>
#include <shaders/shadowmapping/shadows.glsl>


vec3 calculateReflectance2(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic);

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);


vec3 getIrradiance() {
	
	vec4 albedo;
	vec3 norm, emission;
	float roughness, metallic;
	readMaterialProperties(albedo, norm, roughness, metallic, emission, true);
	
	vec3 viewDirection = normalize(cameraPosition - fragPosition.xyz);

	vec3 irradiance = vec3(0.f);
	for(int i = 0; i < plCount; i++) 
		irradiance += calculatePointLight(i, pointLights[i], normal, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);
       
	for(int i = 0; i < dlCount; i++) 
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);

	irradiance += albedo.xyz * emission;
	irradiance = 1.f - exp(-irradiance * 1.f); // Tone mapping
	irradiance = pow(irradiance, vec3(0.4545f)); //Gamma 

	//return albedo;
	return irradiance;
}


//Lighting.....................................................................................................................................

//Reflectance equation with Cook-Torrance BRDF
vec3 calculateReflectance2(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic) {
	
	vec3 dir = lightPosition - fragPosition;
	vec3 lightDirection   = normalize(dir);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float NdotL = doto(normal, lightDirection);     

	float lightDistance = length(dir);
	float attenuation = 1.f / (lightDistance * lightDistance); //Inverse square law
	vec3  radiance = lightDiffuse * attenuation;

	//BRDF
	//float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	//float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(halfwayDirection, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.f) - ks;
    kd *= 1.f - metallic;

	//Lighting equation
	//vec3  nom   = ndf * geo * ks; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	//float denom =  4.f * doto(viewDirection, normal) * NdotL + 0.001f; 
	//vec3  brdf  = nom / denom;

	return (kd * albedo / PI) * radiance * NdotL; //Return only diffuse term
	//return ((kd * albedo / PI + brdf) * radiance) * NdotL; 
}


vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {

	vec3 reflectance = calculateReflectance2(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo, roughness, metallic);
	float shadow = 1.f - calculatePointLightShadows(index, normal, fragPosition);
	
    return shadow * reflectance;
}

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {
	
	vec3 lightDirection = normalize(light.position - fragPosition);

	//Set intensity depending on if object is inside spotlights halo (or outer halo)
	float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.angle - light.outerAngle);
    float intensity = clamp((theta - light.outerAngle) / epsilon, 0.f, 1.f);

	vec3 reflectance = calculateReflectance2(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo, roughness, metallic);

	vec3 coords = vec3(0.f);
	float shadow = 1.f - calculateSpotLightShadows(index, normal, fragPosition, coords);
	float cookie = light.useCookie ? texture(light.cookie, coords.xy).r : 1.f;

    return shadow * reflectance * intensity * cookie;
}

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {
	
	vec3 reflectance = calculateReflectanceDirectional(fragPosition, normal, 
		viewDirection, light.direction, light.diffuse, albedo, roughness, metallic);
	float shadow = 1.f - calculateDirectionalLightShadows(index, normal, fragPosition);
	
    return shadow * reflectance;
}
