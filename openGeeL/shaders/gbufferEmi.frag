#version 430 core

#define OCCLUSION_MIN (1.f / 256.f)

#include <shaders/material.glsl>

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gProperties;

in vec3 normal;
in vec3 fragPosition;
in vec2 textureCoordinates;
in mat3 TBN;

uniform EmissiveMaterial material;


void main() {    

	//Check if materials is actually textured
	float diffFlag = mod(material.mapFlags, 10);
	float specFlag = mod(material.mapFlags / 10, 10);
	float normFlag = mod(material.mapFlags / 100, 10);
	float metaFlag = mod(material.mapFlags / 1000, 10);
	float alphaFlag = mod(material.mapFlags / 10000, 10);
	float emisFlag = mod(material.mapFlags / 100000, 10);
	float occFlag = mod(material.mapFlags / 1000000, 10);

	vec4 diffuse = (diffFlag == 1) 
		? texture(material.diffuse, textureCoordinates) * material.color 
		: material.color;

	if(alphaFlag == 1) {
		diffuse.a = texture(material.alpha, textureCoordinates).r;

		//Discard fragment if alpha value is low
		discard(diffuse.a < 0.5f);
	}
    
	vec3 norm = normalize(normal);
	if(normFlag == 1) {
		norm = texture(material.normal, textureCoordinates).rgb;
		norm = normalize(norm * 2.0f - 1.0f);
		norm = normalize(TBN * norm);
	}

	//Interpret roughness as (1 - specuarlity)
	vec3 speColor = (specFlag == 1) 
		? abs((1.f - float(material.invSpec)) - texture(material.gloss, textureCoordinates).rgb) * material.roughness 
		: vec3(material.roughness);

	float metallic = (metaFlag == 1) 
		? (1.f - texture(material.metal, textureCoordinates).r) * material.metallic
		: material.metallic;

	vec3 emiRGB = (emisFlag == 1) 
		? texture(material.emission, textureCoordinates).rgb * material.emissivity 
		: material.emissivity;

	//Only write intensity of emissivity into gBuffer
	float emissivity = dot(emiRGB, vec3(0.2126f, 0.7152f, 0.0722f));

	float occlusion = (occFlag == 1) 
		? texture(material.occlusion, textureCoordinates).r + OCCLUSION_MIN 
		: 0.f;


	gNormal.rgb = norm;
	gPosition = fragPosition;
	gDiffuse = diffuse;

	gProperties.r = occlusion;
	gProperties.g = emissivity;
	gProperties.b = speColor.r;
	gProperties.a = metallic;
} 
