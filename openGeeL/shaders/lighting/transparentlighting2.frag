#version 430

#include <shaders/helperfunctions.glsl>
#include <shaders/material.glsl>

#include <shaders/lighting/cooktorrance.glsl>


in vec3 normal;
in vec3 fragPosition;
in vec2 texCoords;
in mat3 TBN;

layout (location = 0) out vec4 gPositionRough;
layout (location = 1) out vec4 gNormalMet;
layout (location = 2) out vec4 gDiffuse;

layout (location = 3) out vec4  accumulation;
layout (location = 4) out float revealage;

uniform int plCount;
uniform int dlCount;
uniform int slCount;

#include <shaders/lighting/lights.glsl>

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

uniform Material material;

uniform mat4 projection;
uniform mat4 inverseView;
uniform vec3 origin;


#include <shaders/materialproperties.glsl>
#include <shaders/shadowmapping/shadowsView.glsl>
#include <shaders/lighting/cooktorrancelights.glsl>


void blendCoverage(vec4 premultipliedReflect, vec3 transmit, float z) {
	premultipliedReflect.a *= 1.0 - clamp((transmit.r + transmit.g + transmit.b) * (1.0 / 3.0), 0, 1);
    
	float a = min(1.0, premultipliedReflect.a) * 8.0 + 0.01;
    float b = z * 0.95 + 1.0;
    float w = clamp(a * a * a * 1e8 * b * b * b, 1e-2, 3e2);

	//accumulation = vec4(premultipliedReflect.rgb * premultipliedReflect.a, premultipliedReflect.a) * w;
	accumulation = premultipliedReflect * w;
    revealage    = premultipliedReflect.a;
}


void main() {
	vec4 albedo;
	vec3 norm, emission;
	float roughness, metallic;
	readMaterialProperties(albedo, norm, roughness, metallic, emission, false);
	
	vec3 viewDirection = normalize(-fragPosition.xyz);
	vec3 irradiance = albedo.xyz * emission;

	for(int i = 0; i < plCount; i++) 
		irradiance += calculatePointLight(i, pointLights[i], normal, fragPosition.xyz, viewDirection, albedo, roughness, metallic);
       
	for(int i = 0; i < dlCount; i++) 
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition.xyz, viewDirection, albedo.xyz, roughness, metallic);

	gPositionRough = vec4(fragPosition.xyz, roughness);
	gNormalMet = vec4(norm, metallic);
	gDiffuse = albedo;

	vec3 transmit = vec3(1.f - albedo.a);
	float z = -gl_FragCoord.z;
	blendCoverage(vec4(irradiance, albedo.a), transmit, z);
}

