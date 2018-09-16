#version 430

#define ENABLE_DEFERRED_EMISSIVITY 0

#define POSITION_MAP	gPosition
#define NORMAL_MAP		gNormal
#define DIFFUSE_MAP		gDiffuse
#define PROPERTY_MAP	gProperties

#if (ENABLE_DEFERRED_EMISSIVITY == 1)
#define EMISSION_MAP    gEmission
#endif

#include <shaders/helperfunctions.glsl>
#include <shaders/sampling.glsl>
#include <shaders/lighting/lights.glsl>
#include <shaders/lighting/cooktorrance.glsl>

in vec2 textureCoordinates;

#if (DIFFUSE_SPECULAR_SEPARATION == 0)
layout (location = 0) out vec4 color;
#else
layout (location = 0) out vec4 diffuse;
layout (location = 1) out vec4 specular;
#endif


uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform sampler2D POSITION_MAP;
uniform sampler2D NORMAL_MAP;
uniform sampler2D DIFFUSE_MAP;
uniform sampler2D PROPERTY_MAP;
#if (ENABLE_DEFERRED_EMISSIVITY == 1)
uniform sampler2D EMISSION_MAP;
#endif

#include <shaders/gbufferread.glsl>

uniform mat4 projection;
uniform mat4 inverseView;
uniform vec3 origin;

#if (VOLUMETRIC_LIGHT == 1)
uniform mat4 inverseProjection;
#endif

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];     

#include <shaders/shadowmapping/shadowsView.glsl>
#include <shaders/lighting/cooktorrancelights.glsl>


void main() {
	vec3 fragPosition = readPosition(textureCoordinates);

#if (VOLUMETRIC_LIGHT == 1)
	if(length(fragPosition) <= 0.001f) {
		//Reconstruct position
		vec4 screen = vec4(textureCoordinates, 1.f, 1.f);
		vec4 eye = inverseProjection * 2.f * (screen - vec4(0.5f));

		fragPosition =  eye.xyz / eye.w;
	}
#else
	//Discard pixel if it is not connected to any position in scene (Will be rendered black anyway)
	if(length(fragPosition) <= 0.001f) discard;
#endif

	vec3 viewDirection = normalize(-fragPosition);
    vec3 normal	= readNormal(textureCoordinates);
    vec4 albedo	= readDiffuse(textureCoordinates);

	vec3 emissivity;
	float roughness, metallic;
	readProperties(textureCoordinates, roughness, metallic, emissivity);
	float translucency = readTranslucency(textureCoordinates);


	vec3 irradiance = albedo.rgb * emissivity;

#if (DIFFUSE_SPECULAR_SEPARATION == 0)
	for(int i = 0; i < plCount; i++)
		irradiance += calculatePointLight(i, pointLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic. translucency);
       
	for(int i = 0; i < dlCount; i++)
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic);

	color = vec4(irradiance, 1.f);
#else
	vec3 diff = vec3(0.f);
	vec3 spec = vec3(0.f);

	for(int i = 0; i < plCount; i++)
		calculatePointLight(i, pointLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic, translucency, diff, spec);
       
	for(int i = 0; i < dlCount; i++)
        calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic, diff, spec);

	for(int i = 0; i < slCount; i++)
		calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic, diff, spec);
	
	diffuse  = vec4(diff + irradiance, 1.f);
	specular = vec4(spec, 1.f);
#endif

	
}

