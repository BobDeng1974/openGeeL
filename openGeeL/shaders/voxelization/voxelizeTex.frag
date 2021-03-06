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

layout(binding = 0, rgba8) uniform image3D voxelTexture;


uniform vec2 resolution;
uniform bool drawVoxel;

vec3 getIrradiance();


void main() {

	imageStore(voxelTexture, ivec3(fragPosition * 0.5f), vec4(getIrradiance(), 1.f));
}


//Cook-Torrance lighting............................................................................................................




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
	float roughness, metallic, occlusion;
	readMaterialProperties(texCoords, albedo, norm, roughness, metallic, occlusion, emission, true);
	
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
	float cookie = (light.cookieIndex != 0) ? getCookie(light.cookieIndex, coords.xy) : 1.f;

    return shadow * reflectance * intensity * cookie;
}

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {
	
	vec3 reflectance = calculateReflectanceDirectional(fragPosition, normal, 
		viewDirection, light.direction, light.diffuse, albedo, roughness, metallic);
	float shadow = 1.f - calculateDirectionalLightShadows(index, normal, fragPosition);
	
    return shadow * reflectance;
}


