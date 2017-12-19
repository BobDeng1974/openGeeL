#version 430

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
uniform sampler2D gPositionRoughness;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuse;

uniform sampler2D ssao;
uniform int useSSAO;


void main() {
	vec4 posRough = texture(gPositionRoughness, textureCoordinates);
	vec3 fragPosition = posRough.rgb;
	vec3 position = (inverseView * vec4(fragPosition, 1.f)).xyz;

	//Discard pixel if it is not connected to any position in scene (Will be rendered black anyway)
	if(length(fragPosition) <= 0.001f) return;

	vec4 normMet  = texture(gNormalMet, textureCoordinates);

    vec3 normal		  = normMet.rgb;
    vec4 albedo		  = texture(gDiffuse, textureCoordinates);

	float roughness	  = posRough.a;
	float metallic    = normMet.a;
	float occlusion   = (useSSAO == 1) ? texture(ssao, textureCoordinates).r : 1.f;
	occlusion = (occlusion == 0.f) ? 1.f : clamp(occlusion + OCCLUSION_MIN, 0.f, 1.f);

	vec3  viewDirection = normalize(-fragPosition);

	vec3 ks = calculateFresnelTerm(doto(normal, viewDirection), albedo.rgb, metallic, roughness);
    vec3 kd = vec3(1.f) - ks;
    kd *= 1.f - metallic; //Metallic surfaces don't refract light => nullify kD if metallic

	vec3 ambienceDiffuse = calculateIndirectDiffuse(position, normal, kd, albedo.rgb, occlusion); 
	vec3 ambienceSpecular = occlusion * calculateIndirectSpecularSplitSum(position, normal, viewDirection, 
		albedo.rgb, roughness, metallic);

#if (DIFFUSE_SPECULAR_SEPARATION == 0)
	color = vec4((ambienceDiffuse + ambienceSpecular) * albedo.a, 1.f);
#else
	diffuse  = vec4(ambienceDiffuse  * albedo.a, 1.f);
	specular = vec4(ambienceSpecular * albedo.a, 1.f); 
#endif

}
