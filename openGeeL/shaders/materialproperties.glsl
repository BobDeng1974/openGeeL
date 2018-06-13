
#include <shaders/material.glsl>

#define OCCLUSION_MIN (1.f / 256.f)

//Read values from a previously declared Material 'material'
void readMaterialProperties(vec2 TexCoords, out vec4 albedo, out vec3 norm, out float roughness, out float metallic, out float occlusion, out vec3 emission, bool cutout) {
	
	//Check if materials is actually textured
	float diffFlag  = mod(material.mapFlags, 10);
	float specFlag  = mod(material.mapFlags / 10, 10);
	float normFlag  = mod(material.mapFlags / 100, 10);
	float metaFlag  = mod(material.mapFlags / 1000, 10);
	float alphaFlag = mod(material.mapFlags / 10000, 10);
	float emisFlag  = mod(material.mapFlags / 100000, 10);
	float occFlag   = mod(material.mapFlags / 1000000, 10);
	

	albedo = (diffFlag == 1) 
		? texture(material.diffuse, TexCoords) * material.color 
		: material.color;

	if(alphaFlag == 1) {
		albedo.a = texture(material.alpha, TexCoords).r * albedo.a;

		//Discard fragment if alpha value is low
		discard(cutout && albedo.a < 0.5f);
	}

	norm = normalize(normal);
	if(normFlag == 1) {
		norm = texture(material.normal, TexCoords).rgb;
		norm = normalize(norm * 2.f - 1.f);
		norm = normalize(TBN * norm);
	}

	roughness = (specFlag == 1) 
		? abs((1.f - float(material.invSpec)) - texture(material.gloss, TexCoords).r) * material.roughness 
		: material.roughness; 
	
	metallic = (metaFlag == 1) 
		? 1.f - texture(material.metal, TexCoords).r * material.metallic 
		: material.metallic;
	
	emission = (emisFlag == 1) 
		? texture(material.emission, TexCoords).rgb * material.emissivity 
		: material.emissivity;

	occlusion = (occFlag == 1) 
		? texture(material.occlusion, TexCoords).r + OCCLUSION_MIN 
		: 0.f;

}


float readTranslucency(vec2 TexCoords) {
	float transFlag = mod(material.mapFlags / 10000000, 10);

	return (transFlag == 1)
		? texture(material.translucency, TexCoords).r
		: 1.f;
}
