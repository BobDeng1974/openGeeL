#version 430

#include <renderer/shaders/helperfunctions.glsl>
#include <renderer/shaders/material.glsl>

#include <renderer/lighting/cooktorrance.glsl>


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

#include <renderer/lights/lights.glsl>

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

uniform EmissiveMaterial material;

uniform mat4 projection;
uniform mat4 inverseView;
uniform vec3 origin;


#include <renderer/shaders/materialproperties.glsl>
#include <renderer/shadowmapping/shadowsView.glsl>
#include <renderer/lighting/cooktorrancelights.glsl>


void blendCoverage(vec4 color, float z) {
	float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * 
		pow(1.0 - z * 0.9, 3.0), 1e-2, 3e3);
 
	// Blend Func: GL_ONE, GL_ONE
	accumulation = vec4(color.rgb * color.a, color.a);// * weight;
 
	// Blend Func: GL_ZERO, GL_ONE_MINUS_SRC_ALPHA
	revealage = color.a;

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

	float z = gl_FragCoord.z;
	blendCoverage(vec4(irradiance, albedo.a), z);
}

