#version 430

#include <renderer/shaders/helperfunctions.glsl>
#include <renderer/lighting/cooktorrance.glsl>


struct ReflectionProbe {
	vec3 minPosition;
	vec3 maxPosition;

	samplerCube albedo;
	samplerCube irradiance;
	samplerCube prefilterEnv;
};

in vec2 textureCoordinates;

layout (location = 0) out vec4 color;

uniform sampler2D image;  
uniform sampler2D gPositionRoughness;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuse;

uniform sampler2D ssao;
uniform int useSSAO;

uniform ReflectionProbe skybox;
uniform sampler2D BRDFIntegrationMap;

uniform mat4 inverseView;
uniform vec3 origin;
uniform vec3 ambient;


vec3 calculateIndirectDiffuse(vec3 normal, vec3 kd, vec3 albedo, float occlusion);
vec3 calculateIndirectSpecular(vec3 normal, vec3 view, vec3 albedo, float roughness, float metallic);
vec3 calculateIndirectSpecularSplitSum(vec3 normal, vec3 view, vec3 albedo, float roughness, float metallic);


void main() {
	vec3 base = texture(image, textureCoordinates).rgb;
	vec4 posRough = texture(gPositionRoughness, textureCoordinates);
	vec3 fragPosition = posRough.rgb;

	//Discard pixel if it is not connected to any position in scene (Will be rendered black anyway)
	if(length(fragPosition) <= 0.001f) {
		color = vec4(base, 1.f);
		return;
	}

	vec4 normMet  = texture(gNormalMet, textureCoordinates);

    vec3 normal		  = normMet.rgb;
    vec3 albedo		  = texture(gDiffuse, textureCoordinates).rgb;

	float roughness	  = posRough.a;
	float metallic    = normMet.a;
	float occlusion   = (useSSAO == 1) ? texture(ssao, textureCoordinates).r : 1.f;

	vec3  viewDirection = normalize(-fragPosition);

	vec3 ks = calculateFresnelTerm(doto(normal, viewDirection), albedo, metallic, roughness);
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic; //metallic surfaces don't refract light => nullify kD if metallic

	vec3 ambienceDiffuse = calculateIndirectDiffuse(normal, kd, albedo, occlusion); 
	//vec3 ambienceSpecular = calculateIndirectSpecular(normal, viewDirection, albedo, roughness, metallic);
	vec3 ambienceSpecular = calculateIndirectSpecularSplitSum(normal, viewDirection, albedo, roughness, metallic);

	//color = vec4(ambienceDiffuse, 1.f);
	color = vec4(base + ambienceDiffuse + ambienceSpecular, 1.f);
}

//Lighting.....................................................................................................................................

vec3 calculateIndirectDiffuse(vec3 normal, vec3 kd, vec3 albedo, float occlusion) {

	vec4 normalWorld = inverseView * vec4(origin + normal, 1.f);
	vec3 irradiance = texture(skybox.irradiance, normalWorld.xyz).rgb;
	float l = length(irradiance);
	irradiance *= kd;

	//Use ambient color if no 'real' value is read from irradiance map
	float check = step(0.01f, l);
	irradiance = check * irradiance + (1.f - check) * ambient;

	return irradiance * albedo * occlusion;
}


//Hammersley sequence according to
//http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
vec2 hammersleySeq(int i, int count) {

	uint bits = i;
	bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    float j = float(bits) * 2.3283064365386963e-10;

	return vec2(float(i) / float(count), j);
}


vec3 generateSampledVector(float roughness, vec2 samp) {
	float e1 = samp.x;
	float e2 = samp.y;

	float theta = atan((roughness * sqrt(e1)) / sqrt(1.f - e2));
	//float theta = atan((roughness * roughness * sqrt(e1)) / sqrt(1.f - e2));
	float phi   = 2.f * PI * e2;

	return vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}

vec3 calculateIndirectSpecular(vec3 normal, vec3 view, vec3 albedo, float roughness, float metallic) {

	vec3 normalWorld = normalize((inverseView * vec4(origin + normal, 1.f)).xyz);
	vec3 viewWorld = normalize((inverseView * vec4(origin + view, 1.f)).xyz);
	vec3 reflection = normalize(reflect(-viewWorld, normalWorld));
	vec3 right = cross(reflection, normalWorld);
	vec3 up = cross(reflection, right);

	float mipmapHeuristic = 150 * roughness * roughness;
	float NoV = doto(normalWorld, viewWorld);

	vec3 radiance = vec3(0.f);
	int sampleCount = 1;
	for(int i = 0; i < sampleCount; i++) {
		vec3 sampleVector = generateSampledVector(roughness, hammersleySeq(i, sampleCount));
		sampleVector = sampleVector.x * right + sampleVector.y * up + sampleVector.z * reflection; //To world coordinates
		sampleVector = normalize(sampleVector);

		vec3 halfway = normalize(sampleVector + viewWorld);
		float cosT = doto(sampleVector, normalWorld);
		cosT = clamp(cosT, 0.f, 1.f);
		float sinT = sqrt(1.f - cosT * cosT);

		float theta = doto(halfway, viewWorld);
		vec3  fresnel = calculateFresnelTerm(theta, albedo, metallic, roughness);

		float geo = calculateGeometryFunctionSmith(normalWorld, viewWorld, sampleVector, roughness);

		float denom =  1.f / (4.f * NoV * doto(halfway, normalWorld) + 0.001f); 

		radiance += textureLod(skybox.albedo, sampleVector, mipmapHeuristic).rgb * geo * fresnel * sinT * denom;
	}
	
	float samp = 1.f / float(sampleCount);
	radiance *= samp;

	//Factor in pseudo NDF if only one sample is taken
	float single = step(sampleCount, 2);
	return radiance * (1.f - single) + 
		radiance * single * (1.f - roughness);
}

const float ROUGHNESS_LOD = 4; //Amount of roughness levels in pre-filtered environment map 

vec3 calculateIndirectSpecularSplitSum(vec3 normal, vec3 view, vec3 albedo, float roughness, float metallic) {
	vec3 normalWorld = normalize((inverseView * vec4(origin + normal, 1.f)).xyz);
	vec3 viewWorld = normalize((inverseView * vec4(origin + view, 1.f)).xyz);
	vec3 reflection = normalize(reflect(-viewWorld, normalWorld));
	float NdotV = doto(normalWorld, viewWorld);

	vec3 prefilteredColor = textureLod(skybox.prefilterEnv, reflection,  roughness * ROUGHNESS_LOD).rgb; 
	vec2 brdfInt = texture(BRDFIntegrationMap, vec2(NdotV, roughness)).rg;
	brdfInt = brdfInt;

	vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

	//Main splitsum integral
	return prefilteredColor * (F0 * brdfInt.x + brdfInt.y);
}

