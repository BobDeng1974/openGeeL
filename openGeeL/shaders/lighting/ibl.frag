#version 430

#define POSITION_MAP	gPositionRoughness
#define NORMAL_MAP		gNormalMet
#define DIFFUSE_MAP		gDiffuse
#define OCCLUSION_MAP   ssao;

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

uniform sampler2D OCCLUSION_MAP;
uniform int useSSAO;


void main() {
	vec4 posRough = texture(POSITION_MAP, textureCoordinates);
	vec3 fragPosition = posRough.rgb;
	vec3 position = (inverseView * vec4(fragPosition, 1.f)).xyz;

	//Discard pixel if it is not connected to any position in scene (Will be rendered black anyway)
	if(length(fragPosition) <= 0.001f) return;

	vec4 normMet  = texture(NORMAL_MAP, textureCoordinates);

    vec3 normal		  = normMet.rgb;
    vec4 albedo		  = texture(DIFFUSE_MAP, textureCoordinates);

	float roughness	  = posRough.a;
	float metallic    = normMet.a;
	float occlusion   = (useSSAO == 1) ? texture(OCCLUSION_MAP, textureCoordinates).r : 1.f;
	occlusion = (occlusion == 0.f) ? 1.f : clamp(occlusion + OCCLUSION_MIN, 0.f, 1.f);

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
