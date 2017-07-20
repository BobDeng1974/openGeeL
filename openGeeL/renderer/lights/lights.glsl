
struct PointLight {
	samplerCube shadowMap;

    vec3 position;
    vec3 diffuse;

	float shadowIntensity;
	float bias;
	float farPlane;

	int resolution;
	float scale;
	int type; //0: No 1: Hard 2: Soft shadow
};

struct SpotLight {
	sampler2D shadowMap;
	sampler2D cookie;
	mat4 lightTransform;

    vec3 position;
    vec3 direction;
	vec3 diffuse;

    float angle;
    float outerAngle;
	float shadowIntensity;
	float bias;

	int resolution;
	float scale;
	int type; //0: No 1: Hard 2: Soft shadow
	bool useCookie;
};

const int DIRECTIONAL_SHADOWMAP_COUNT = 4;

struct DirectionalLight {
	sampler2D shadowMap;
	float cascadeEndClip[DIRECTIONAL_SHADOWMAP_COUNT];
	mat4 lightTransforms[DIRECTIONAL_SHADOWMAP_COUNT];

	vec3 direction;
    vec3 diffuse;

	float bias;
	int type; //0: No 1: Hard 2: Soft shadow
};