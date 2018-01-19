
struct Material {
	sampler2D diffuse;
	sampler2D gloss;
	sampler2D normal;
	sampler2D metal;
	sampler2D alpha;
	sampler2D emission;
	sampler2D occlusion;

	int mapFlags;
	bool invSpec;

	float roughness;
	float metallic;

	vec4  color;
	vec3 emissivity;
};