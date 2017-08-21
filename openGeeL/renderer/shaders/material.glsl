

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D metal;
	sampler2D alpha;

	int mapFlags;
	bool invSpec;

	float roughness;
	float metallic;
	vec4  color;
};


struct EmissiveMaterial {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D metal;
	sampler2D alpha;
	sampler2D emission;

	int mapFlags;
	bool invSpec;

	float roughness;
	float metallic;

	vec4  color;
	vec3 emissivity;
};