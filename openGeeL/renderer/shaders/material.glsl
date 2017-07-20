

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


struct EmissiveMaterial {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D metal;

	int mapFlags;
	float roughness;
	float metallic;

	vec4  color;
	vec3 emissivity;
};