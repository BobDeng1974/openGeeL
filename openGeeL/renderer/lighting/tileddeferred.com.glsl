#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(binding = 0, rgba16f) uniform image2D target;

uniform vec2 resolution;

#include <renderer/shaders/helperfunctions.glsl>
#include <renderer/shaders/sampling.glsl>
#include <renderer/lights/lights.glsl>
#include <renderer/lighting/cooktorrance.glsl>

uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform sampler2D gPositionRoughness;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuse;
uniform sampler2D gEmissivity;

uniform bool useEmissivity;

uniform mat4 projection;
uniform mat4 inverseView;
uniform vec3 origin;

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];     

#include <renderer/shadowmapping/shadowsView.glsl>
#include <renderer/lighting/cooktorrancelights.glsl>


void main() {
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 textureCoordinates = gl_GlobalInvocationID.xy / resolution;


	vec4 posRough = texture2D(gPositionRoughness, textureCoordinates);
	vec3 fragPosition = posRough.rgb;

	vec4 normMet  = texture2D(gNormalMet, textureCoordinates);

    vec3 normal		= normMet.rgb;
    vec4 albedo		= texture2D(gDiffuse, textureCoordinates);
	vec3 emissivity = useEmissivity ? texture2D(gEmissivity, textureCoordinates).rgb : vec3(0.f);

	float roughness	= posRough.a;
	float metallic  = normMet.a;

	vec3  viewDirection = normalize(-fragPosition);


	vec3 irradiance = albedo.rgb * emissivity;
	for(int i = 0; i < plCount; i++)
		irradiance += calculatePointLight(i, pointLights[i], normal, fragPosition, viewDirection, albedo, roughness, metallic);
       
	for(int i = 0; i < dlCount; i++)
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic);


	imageStore(target, coords, vec4(irradiance, 1.f));
}
