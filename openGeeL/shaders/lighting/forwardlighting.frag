#version 430

#define FOG 1

#include <shaders/helperfunctions.glsl>
#include <shaders/material.glsl>

#include <shaders/lighting/cooktorrance.glsl>

in vec3 normal;
in vec3 fragPosition;
in vec2 texCoords;
in mat3 TBN;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gProperties;

#if (DIFFUSE_SPECULAR_SEPARATION == 0)
layout (location = 4) out vec4 color;
#else
layout (location = 4) out vec4 diffuse;
layout (location = 5) out vec4 specular;
#endif

uniform int plCount;
uniform int dlCount;
uniform int slCount;

#include <shaders/lighting/lights.glsl>

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

uniform EmissiveMaterial material;

uniform mat4 projection;
uniform mat4 inverseView;
uniform vec3 origin;

uniform float fogFalloff = 200.f;


#include <shaders/materialproperties.glsl>
#include <shaders/shadowmapping/shadowsView.glsl>
#include <shaders/lighting/cooktorrancelights.glsl>


void main() {
	vec4 albedo;
	vec3 norm, emission;
	float roughness, metallic;
	readMaterialProperties(albedo, norm, roughness, metallic, emission, false);
	

	vec3 viewDirection = normalize(-fragPosition.xyz);

	gPosition = fragPosition.xyz;
	gNormal = vec4(norm, 0.f);
	gDiffuse = albedo;
	gProperties = vec4(0.f, 0.f, roughness, metallic);


#if (FOG == 1)
	float fogFactor = 1.f - clamp(abs(fragPosition.z) / fogFalloff, 0.f, 1.f);
#endif

	vec3 irradiance = albedo.xyz * emission;

#if (DIFFUSE_SPECULAR_SEPARATION == 0)
	for(int i = 0; i < plCount; i++) 
		irradiance += calculatePointLight(i, pointLights[i], norm, fragPosition.xyz, viewDirection, albedo, roughness, metallic);
       
	for(int i = 0; i < dlCount; i++) 
        irradiance += calculateDirectionaLight(i, directionalLights[i], norm, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], norm, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);

#if (FOG == 1)
	color = vec4(irradiance, albedo.a) * fogFactor;
#else
	color = vec4(irradiance, albedo.a);
#endif

	

#else
	vec3 diff = vec3(0.f);
	vec3 spec = vec3(0.f);

	for(int i = 0; i < plCount; i++) 
		irradiance += calculatePointLight(i, pointLights[i], norm, fragPosition.xyz, viewDirection, albedo, roughness, metallic);
       
	for(int i = 0; i < dlCount; i++) 
        irradiance += calculateDirectionaLight(i, directionalLights[i], norm, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], norm, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);

#if (FOG == 1)
	diffuse  = vec4(diff + irradiance, albedo.a) * fogFactor;
	specular = vec4(spec, albedo.a) * fogFactor;
#else
	diffuse = vec4(diff + irradiance, albedo.a);
	specular = vec4(spec, albedo.a);
#endif

#endif

}

