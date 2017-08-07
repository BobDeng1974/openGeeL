
#include <renderer/shaders/material.glsl>

//Read values from a previously declared Material 'material'
void readMaterialProperties(out vec3 albedo, out vec3 norm, out float roughness, out float metallic) {
	
	//Check if materials is actually textured
	float diffFlag = mod(material.mapFlags, 10);
	float specFlag = mod(material.mapFlags / 10, 10);
	float normFlag = mod(material.mapFlags / 100, 10);
	float metaFlag = mod(material.mapFlags / 1000, 10);

	vec4 diffuse = (diffFlag == 1) ? texture(material.diffuse, texCoords) : material.color;

	//Discard fragment if alpha value is very low
	discard(diffuse.a < 0.1f);

	norm = normalize(normal);
	if(normFlag == 1) {
		norm = texture(material.normal, texCoords).rgb;
		norm = normalize(norm * 2.f - 1.f);
		norm = normalize(TBN * norm);
	}

	albedo = diffuse.rgb;
	roughness = (specFlag == 1) ? 1.f - texture(material.specular, texCoords).r : material.roughness; 
	metallic = (metaFlag == 1) ? texture(material.metal, texCoords).r : material.metallic;
}
