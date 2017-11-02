
#include <renderer/shaders/material.glsl>

//Read values from a previously declared Material 'material'
void readMaterialProperties(out vec4 albedo, out vec3 norm, out float roughness, out float metallic, out vec3 emission) {
	
	//Check if materials is actually textured
	float diffFlag = mod(material.mapFlags, 10);
	float specFlag = mod(material.mapFlags / 10, 10);
	float normFlag = mod(material.mapFlags / 100, 10);
	float metaFlag = mod(material.mapFlags / 1000, 10);
	float alphaFlag = mod(material.mapFlags / 10000, 10);
	float emisFlag = mod(material.mapFlags / 100000, 10);

	vec4 diffuse = (diffFlag == 1) ? texture(material.diffuse, texCoords) * material.color : material.color;

	if(alphaFlag == 1) {
		diffuse.a = texture(material.alpha, texCoords).r;

		//Discard fragment if alpha value is low
		discard(diffuse.a < 0.5f);
	}

	norm = normalize(normal);
	if(normFlag == 1) {
		norm = texture(material.normal, texCoords).rgb;
		norm = normalize(norm * 2.f - 1.f);
		norm = normalize(TBN * norm);
	}

	albedo = diffuse;
	roughness = (specFlag == 1) ? abs((1.f - float(material.invSpec)) - texture(material.specular, texCoords).r) * material.roughness : material.roughness; 
	metallic = (metaFlag == 1) ? 1.f - texture(material.metal, texCoords).r * material.metallic : material.metallic;
	emission = (emisFlag == 1) ? texture(material.emission, texCoords).rgb * material.emissivity : material.emissivity;
}
