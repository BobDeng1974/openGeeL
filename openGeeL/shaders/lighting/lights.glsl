
#define USE_CASCASDED_MAP 0
#define VOLUMETRIC_LIGHT 0


struct PointLight {
	unsigned int shadowmapIndex;

    vec3 position;
    vec3 diffuse;

	float shadowIntensity;
	float bias;
	float farPlane;
	float radius;
	
#if (VOLUMETRIC_LIGHT == 1)
	float volumetricStrength;
	float volumetricDensity;
#endif

	int resolution;
	float scale;
	int type; //1: Hard 2: Soft shadow
};

struct SpotLight {
	unsigned int shadowmapIndex;
	unsigned int cookieIndex;

	mat4 lightTransform;

    vec3 position;
    vec3 direction;
	vec3 diffuse;

    float angle;
    float outerAngle;
	float shadowIntensity;
	float bias;
	float radius;

	int resolution;
	float scale;
	int type; //1: Hard 2: Soft shadow
};

#if (USE_CASCASDED_MAP == 1)
const int DIRECTIONAL_SHADOWMAP_COUNT = 4;
#endif

struct DirectionalLight {
	unsigned int shadowmapIndex;

#if (USE_CASCASDED_MAP == 1)
	float cascadeEndClip[DIRECTIONAL_SHADOWMAP_COUNT];
	mat4 lightTransforms[DIRECTIONAL_SHADOWMAP_COUNT];
#else
	mat4 lightTransform;

	int resolution;
	float scale;
#endif

	vec3 direction;
    vec3 diffuse;

	float shadowIntensity;
	float bias;
	int type; //1: Hard 2: Soft shadow
};


#define MAX_POINTLIGHTS 5
#define MAX_SPOTLIGHTS 5
#define MAX_DIRECTIONALLIGHTS 1

#define MAX_SHADOWMAPS 2
#define MAX_SHADOWCUBES 2
#define MAX_COOKIES 1


uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform PointLight pointLights[MAX_POINTLIGHTS];
uniform DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
uniform SpotLight spotLights[MAX_SPOTLIGHTS];


uniform sampler2D shadowMaps[MAX_SHADOWMAPS];
uniform samplerCube shadowCubes[MAX_SHADOWCUBES];
uniform sampler2D cookieMaps[MAX_COOKIES];


float getShadowDepth2D(unsigned int index, vec2 coordinates) {
	return texture(shadowMaps[index - 1], coordinates).r;
}

float getShadowDepthCube(unsigned int index, vec3 direction) {
	return texture(shadowCubes[index - 1], direction).r;
}

float getCookie(unsigned int index, vec2 coordinates) {
	return texture(cookieMaps[index - 1], coordinates).r;
}


