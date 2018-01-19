#version 430

#define POSITION_MAP	gPosition
#define NORMAL_MAP		gNormal
#define DIFFUSE_MAP		gDiffuse
#define PROPERTY_MAP	gProperties

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

#include <shaders/gbufferread.glsl>

uniform bool useEmissivity;

uniform mat4 projection;
uniform mat4 inverseView;
uniform vec3 origin;

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];     

#include <shaders/shadowmapping/shadowsView.glsl>
#include <shaders/lighting/cooktorrancelights.glsl>




vec3 calculateVolumetricLightColor(vec3 fragPos, vec3 lightPosition, vec3 lightColor, float density);


void main() {
	vec3 fragPosition = readPosition(textureCoordinates);

	//Discard pixel if it is not connected to any position in scene (Will be rendered black anyway)
	discard(length(fragPosition) <= 0.001f);

    vec3 normal		= readNormal(textureCoordinates);
    vec4 albedo		= readDiffuse(textureCoordinates);

	vec4 properties = readProperties(textureCoordinates);
	vec3 emissivity = vec3(properties.g);
	float roughness = properties.b;
	float metallic = properties.a;


	vec3  viewDirection = normalize(-fragPosition);


	vec3 irradiance = albedo.rgb * emissivity;

#if (DIFFUSE_SPECULAR_SEPARATION == 0)
	for(int i = 0; i < plCount; i++)
		irradiance += calculatePointLight(i, pointLights[i], normal, fragPosition, viewDirection, albedo, roughness, metallic);
       
	for(int i = 0; i < dlCount; i++)
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic);

	color = vec4(irradiance, 1.f);
#else
	vec3 diff = vec3(0.f);
	vec3 spec = vec3(0.f);

	for(int i = 0; i < plCount; i++)
		calculatePointLight(i, pointLights[i], normal, fragPosition, viewDirection, albedo, roughness, metallic, diff, spec);
       
	for(int i = 0; i < dlCount; i++)
        calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic, diff, spec);

	for(int i = 0; i < slCount; i++)
		calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic, diff, spec);
	
	diffuse  = vec4(diff + irradiance, 1.f);
	specular = vec4(spec, 1.f);
#endif

	
}

//Volumetric light......................................................................................................................

vec3 calculateVolumetricLightColor(vec3 fragPos, vec3 lightPosition, vec3 lightColor, float density) {

	float lightInView = step(lightPosition.z, 0.f);

	//Find shortest path from light position to viewing vector
	float depth = length(fragPos);
	vec3 n = normalize(fragPos);
	vec3 a_p = -lightPosition;
	float projN = dot(a_p, n);
	vec3 shortestPath = a_p - projN * n;

	//Pick shortest path OR fragPosition if shortestPath is too far away
	float mini = step(depth, -projN);
	float a = (1.f - mini) * length(shortestPath);
	float b =  mini * distance(fragPos, lightPosition);
	float dist = a + b + 0.0001f;
	float attenuation = 1.f / (dist * dist);

	return lightColor * attenuation * density * lightInView;
}

