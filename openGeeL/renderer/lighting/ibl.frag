#version 430

#include <renderer/shaders/helperfunctions.glsl>
#include <renderer/shaders/samplingvector.glsl>
#include <renderer/lighting/cooktorrance.glsl>


struct ReflectionProbe {
	bool global;
	vec3 centerPosition;
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


//Translate given vector into space of reflection probe
vec3 getProbeVector(vec3 vector, vec3 fragPosition);

vec3 calculateIndirectDiffuse(vec3 position, vec3 normal, vec3 kd, vec3 albedo, float occlusion);
vec3 calculateIndirectSpecular(vec3 position, vec3 normal, vec3 view, vec3 albedo, float roughness, float metallic);
vec3 calculateIndirectSpecularSplitSum(vec3 position, vec3 normal, vec3 view, vec3 albedo, float roughness, float metallic);


void main() {
	//vec3 base = texture(image, textureCoordinates).rgb;
	vec4 posRough = texture(gPositionRoughness, textureCoordinates);
	vec3 fragPosition = posRough.rgb;
	vec3 position = (inverseView * vec4(fragPosition, 1.f)).xyz;

	//Discard pixel if it is not connected to any position in scene (Will be rendered black anyway)
	if(length(fragPosition) <= 0.001f) {
		//color = vec4(base, 1.f);
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
    vec3 kd = vec3(1.f) - ks;
    kd *= 1.f - metallic; //metallic surfaces don't refract light => nullify kD if metallic

	vec3 ambienceDiffuse = calculateIndirectDiffuse(position, normal, kd, albedo, occlusion); 
	//vec3 ambienceSpecular = calculateIndirectSpecular(position, normal, viewDirection, albedo, roughness, metallic);
	vec3 ambienceSpecular = calculateIndirectSpecularSplitSum(position, normal, viewDirection, albedo, roughness, metallic);

	color = vec4(ambienceDiffuse + ambienceSpecular, 1.f);
}

//Lighting.....................................................................................................................................

vec3 calculateIndirectDiffuse(vec3 position, vec3 normal, vec3 kd, vec3 albedo, float occlusion) {

	vec3 normalWorld = (inverseView * vec4(origin + normal, 1.f)).xyz;
	normalWorld = getProbeVector(normalWorld, position);

	vec3 irradiance = texture(skybox.irradiance, normalWorld).rgb;
	float l = length(irradiance);
	irradiance *= kd;

	//Use ambient color if no 'real' value is read from irradiance map
	float check = step(0.01f, l);
	irradiance = check * irradiance + (1.f - check) * ambient;

	return irradiance * albedo * occlusion;
}


vec3 calculateIndirectSpecular(vec3 position, vec3 normal, vec3 view, vec3 albedo, float roughness, float metallic) {

	vec3 normalWorld = normalize((inverseView * vec4(origin + normal, 1.f)).xyz);
	vec3 viewWorld = normalize((inverseView * vec4(origin + view, 1.f)).xyz);
	vec3 reflection = normalize(reflect(-viewWorld, normalWorld));
	reflection = getProbeVector(reflection, position); 
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



vec3 calculateIndirectSpecularSplitSum(vec3 position, vec3 normal, vec3 view, vec3 albedo, float roughness, float metallic) {
	vec3 normalWorld = normalize((inverseView * vec4(origin + normal, 1.f)).xyz);
	vec3 viewWorld = normalize((inverseView * vec4(origin + view, 1.f)).xyz);
	vec3 reflection = normalize(reflect(-viewWorld, normalWorld));
	reflection = getProbeVector(reflection, position);


	float NdotV = doto(normalWorld, viewWorld);

	vec3 prefilteredColor = textureLod(skybox.prefilterEnv, reflection,  roughness * ROUGHNESS_LOD).rgb; 
	vec2 brdfInt = texture(BRDFIntegrationMap, vec2(NdotV, roughness)).rg;
	brdfInt = brdfInt;

	vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

	//Main splitsum integral
	return prefilteredColor * (F0 * brdfInt.x + brdfInt.y);
}


//Helper functions for probe space computations....................................................................

const float epsilon = 0.000001f;
const float FLOAT_MAX = 100000.f;

float planeIntersection(vec3 rayO, vec3 rayD, vec3 planeO, vec3 planeN) {
	float v = dot(planeN, rayD) + epsilon;
	vec3 a = planeO - rayO;
	float d = dot(a, planeN) / v;

	float pick = step(epsilon, d);
	return (pick * d) + ((1.f - pick) * FLOAT_MAX);
}

float getBorderDistance(vec3 position, vec3 direction, vec3 center) {

	float distances[6];
	distances[0] = planeIntersection(position, direction, skybox.minPosition, vec3(1.f, 0.f, 0.f));
	distances[1] = planeIntersection(position, direction, skybox.minPosition, vec3(0.f, 1.f, 0.f));
	distances[2] = planeIntersection(position, direction, skybox.minPosition, vec3(0.f, 0.f, 1.f));
	distances[3] = planeIntersection(position, direction, skybox.maxPosition, vec3(1.f, 0.f, 0.f));
	distances[4] = planeIntersection(position, direction, skybox.maxPosition, vec3(0.f, 1.f, 0.f));
	distances[5] = planeIntersection(position, direction, skybox.maxPosition, vec3(0.f, 0.f, 1.f));

	float minDist = FLOAT_MAX;
	for(int i = 0; i < 6; i++)
		minDist = min(minDist, distances[i]);

	return minDist;
}

vec3 getProbeVector(vec3 vector, vec3 position) {
	//Return vector again if skybox is global (and has no local position)
	if(skybox.global) return vector;

	float dist = getBorderDistance(position, vector, skybox.centerPosition);
	vec3 target = position + dist * vector;

	return normalize(target - skybox.centerPosition);
}