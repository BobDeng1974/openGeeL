#version 430 core

layout (location = 0) out vec4 gPositionRough;
layout (location = 1) out vec4 gNormalMet;
layout (location = 2) out vec4 gDiffuse;

in vec3 normal;
in vec3 fragPosition;
in vec2 textureCoordinates;
in mat3 TBN;


struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D metal;

	int mapFlags;
	float roughness;
	float metallic;
	vec4  color;
};

uniform Material material;


void main() {    

	//Check if materials is actually textured
	float diffFlag = mod(material.mapFlags, 10);
	float specFlag = mod(material.mapFlags / 10, 10);
	float normFlag = mod(material.mapFlags / 100, 10);
	float metaFlag = mod(material.mapFlags / 1000, 10);

	vec4 diffuse = (diffFlag == 1) ? texture(material.diffuse, textureCoordinates) : material.color;

	//Discard fragment if alpha value is very low
	//discard(diffuse.a < 0.1f);
		
	gPositionRough.xyz = fragPosition;
    
	vec3 norm = normalize(normal);
	if(normFlag == 1) {
		norm = texture(material.normal, textureCoordinates).rgb;
		norm = normalize(norm * 2.0f - 1.0f);
		norm = normalize(TBN * norm);
	}

	//Interpret roughness as (1 - specuarlity)
	vec3 speColor = (specFlag == 1) ? 1.f - texture(material.specular, textureCoordinates).rgb : vec3(material.roughness);
	float metallic = (metaFlag == 1) ? texture(material.metal, textureCoordinates).r : material.metallic;

	gNormalMet.rgb = norm;
	gNormalMet.a = metallic;
	
	gDiffuse = diffuse;

	gPositionRough.a = speColor.r;

} 
