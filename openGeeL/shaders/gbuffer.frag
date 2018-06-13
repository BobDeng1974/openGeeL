#version 430 core

#define ENABLE_DEFERRED_EMISSIVITY 0

#include <shaders/material.glsl>

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gProperties;
#if (ENABLE_DEFERRED_EMISSIVITY == 1)
layout (location = 4) out float gEmission;
#endif

in vec3 normal;
in vec3 fragPosition;
in vec2 textureCoordinates;
in mat3 TBN;

uniform Material material;
uniform uint id;

#include <shaders/materialproperties.glsl>


void main() {    
	vec4 albedo;
	vec3 norm, emission;
	float roughness, metallic, occlusion;
	readMaterialProperties(textureCoordinates, albedo, norm, roughness, metallic, occlusion, emission, true);

	gNormal.rgb = norm;
	gNormal.a = uintBitsToFloat(id);

	gPosition = fragPosition;
	gDiffuse = albedo;

	gProperties.r = roughness;
	gProperties.g = metallic;
	gProperties.b = readTranslucency(textureCoordinates);
	gProperties.a = occlusion;

#if (ENABLE_DEFERRED_EMISSIVITY == 1)
	gEmission = dot(emission, vec3(0.2126f, 0.7152f, 0.0722f));
#endif

} 
