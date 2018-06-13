
struct Material {
	sampler2D diffuse;
	sampler2D gloss;
	sampler2D normal;
	sampler2D metal;
	sampler2D alpha;
	sampler2D emission;
	sampler2D occlusion;
	sampler2D translucency;

	int mapFlags;
	bool invSpec;

	float roughness;
	float metallic;
	float translucencyFactor;

	vec4  color;
	vec3 emissivity;
};