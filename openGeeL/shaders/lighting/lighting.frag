#version 430

#include <shaders/helperfunctions.glsl>
#include <shaders/material.glsl>

#include <shaders/lighting/cooktorrance.glsl>


in vec3 normal;
in vec3 fragPosition;
in vec2 texCoords;
in mat3 TBN;

out vec4 color;

uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform bool gammaCorrection;

#include <shaders/lighting/lights.glsl>

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

uniform Material material;
uniform vec3 cameraPosition;


#include <shaders/materialproperties.glsl>
#include <shaders/shadowmapping/shadows.glsl>


vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);


void main() {
	vec4 albedo;
	vec3 norm, emission;
	float roughness, metallic, occlusion;
	readMaterialProperties(texCoords, albedo, norm, roughness, metallic, occlusion, emission, false);
	
	vec3 viewDirection = normalize(cameraPosition - fragPosition.xyz);
	vec3 irradiance = albedo.xyz * emission;

	for(int i = 0; i < plCount; i++) 
		irradiance += calculatePointLight(i, pointLights[i], norm, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);
       
	for(int i = 0; i < dlCount; i++) 
        irradiance += calculateDirectionaLight(i, directionalLights[i], norm, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], norm, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);

	color = vec4(gammaCorrection ? pow(irradiance, vec3(0.4545f)) : irradiance, albedo.a);
}


vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
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

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
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