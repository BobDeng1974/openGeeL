#version 430

#define POSITION_MAP	gPosition
#define NORMAL_MAP		gNormal
#define DIFFUSE_MAP		gDiffuse
#define PROPERTY_MAP	gProperties

#include <shaders/lighting/iblcore.glsl>


#define OCCLUSION_MIN (1.f / 256.f)


in vec2 textureCoordinates;

#if (DIFFUSE_SPECULAR_SEPARATION == 0)
layout (location = 0) out vec4 color;
#else
layout (location = 0) out vec4 diffuse;
layout (location = 1) out vec4 specular;
#endif

uniform sampler2D image;  
uniform sampler2D POSITION_MAP;
uniform sampler2D NORMAL_MAP;
uniform sampler2D DIFFUSE_MAP;
uniform sampler2D PROPERTY_MAP;

uniform int useSSAO;

#include <shaders/gbufferread.glsl>


void main() {
	vec3 fragPosition = readPosition(textureCoordinates);
	vec3 position = (inverseView * vec4(fragPosition, 1.f)).xyz;

	//Discard pixel if it is not connected to any position in scene (Will be rendered black anyway)
	if(length(fragPosition) <= 0.001f) return;

    vec3 normal		  = readNormal(textureCoordinates);
    vec4 albedo		  = readDiffuse(textureCoordinates);

	vec4 properties = readProperties(textureCoordinates);
	float roughness = properties.b;
	float metallic = properties.a;


	float occlusion   = 1.f;
	//occlusion = (occlusion == 0.f) ? 1.f : clamp(occlusion + OCCLUSION_MIN, 0.f, 1.f);

	vec3  viewDirection = normalize(-fragPosition);

	vec3 ks = calculateFresnelTerm(doto(normal, viewDirection), albedo.rgb, metallic, roughness);
    vec3 kd = vec3(1.f) - ks;
    kd *= 1.f - metallic; //Metallic surfaces don't refract light => nullify kD if metallic

	vec3 ambienceDiffuse = calculateIndirectDiffuse(position, normal, kd, albedo.rgb, occlusion); 
	vec3 ambienceSpecular = occlusion * calculateIndirectSpecularSplitSum(position, normal, viewDirection, 
		albedo.rgb, roughness, metallic);

#if (DIFFUSE_SPECULAR_SEPARATION == 0)
	color = vec4(ambienceDiffuse + ambienceSpecular, 1.f);
#else
	diffuse  = vec4(ambienceDiffuse, 1.f);
	specular = vec4(ambienceSpecular, 1.f); 
#endif

}
