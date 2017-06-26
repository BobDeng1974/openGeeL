
#version 430


in vec4 fragPosition;
in vec3 normal;
in vec2 texCoords;
in mat3 TBN;

flat in int  axis;
flat in vec4 AABB;

layout(location = 0) out vec4 gl_FragColor;

layout(binding = 0, rgba8) uniform image3D voxelTexture;


uniform vec2 resolution;
uniform bool drawVoxel;

vec3 getIrradiance();


void main() {

	imageStore(voxelTexture, ivec3(fragPosition * 0.5f), vec4(getIrradiance(), 1.f));
}


//Cook-Torrance lighting............................................................................................................

const float PI = 3.14159265359;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D metal;

	int mapFlags;
	float roughness;
	float metallic;
	vec4  color;
};

struct PointLight {
	samplerCube shadowMap;

    vec3 position;
    vec3 diffuse;

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

uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

uniform Material material;
uniform vec3 cameraPosition;

//Return dot(a,b) >= 0
float doto(vec3 a, vec3 b);

void readMaterialProperties(out vec3 albedo, out vec3 norm, out float roughness, out float metallic);

vec3 calculateReflectance(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic);

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness);


float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition);
float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition, inout vec3 coords);
float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition);


vec3 getIrradiance() {
	
	vec3 albedo, norm;
	float roughness, metallic;
	readMaterialProperties(albedo, norm, roughness, metallic);
	
	vec3 viewDirection = normalize(cameraPosition - fragPosition.xyz);

	vec3 irradiance = vec3(0.f);
	for(int i = 0; i < plCount; i++) 
		irradiance += calculatePointLight(i, pointLights[i], normal, fragPosition.xyz, viewDirection, albedo, roughness, metallic);
       
	for(int i = 0; i < dlCount; i++) 
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition.xyz, viewDirection, albedo, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition.xyz, viewDirection, albedo, roughness, metallic);

	irradiance = 1.f - exp(-irradiance * 1.f); // Tone mapping
	irradiance = pow(irradiance, vec3(0.4545f)); //Gamma 

	//return albedo;
	return irradiance;
}


void readMaterialProperties(out vec3 albedo, out vec3 norm, out float roughness, out float metallic) {
	
	//Check if materials is actually textured
	float diffFlag = mod(material.mapFlags, 10);
	float specFlag = mod(material.mapFlags / 10, 10);
	float normFlag = mod(material.mapFlags / 100, 10);
	float metaFlag = mod(material.mapFlags / 1000, 10);

	vec4 diffuse = (diffFlag == 1) ? texture(material.diffuse, texCoords) : material.color;

	//Discard fragment if alpha value is very low
	discard(diffuse.a < 0.1f);

	norm = normalize(normal);
	if(normFlag == 1) {
		norm = texture(material.normal, texCoords).rgb;
		norm = normalize(norm * 2.f - 1.f);
		norm = normalize(TBN * norm);
	}

	albedo = diffuse.rgb;
	roughness = (specFlag == 1) ? 1.f - texture(material.specular, texCoords).r : material.roughness; 
	metallic = (metaFlag == 1) ? texture(material.metal, texCoords).r : material.metallic;
}


//Lighting.....................................................................................................................................

//Compute fresnel term with Fresnel-Schlick approximation
vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness) {
	vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

	//Simplified term withouth roughness included
    //return F0 + (1.0f - F0) * pow(1.0f - theta, 5.0f);
	vec3 fres = F0 + (max(vec3(1.f - roughness), F0) - F0) * pow(1.f - theta, 5.f);
	return clamp(fres, 0.f, 1.f);
}

//Trowbridge-Reitz GGX normal distribution function
float calculateNormalDistrubution(vec3 normal, vec3 halfway, float roughness) {
    float a = roughness * roughness;
    float NdotH  = doto(normal, halfway);
    float NdotH2 = NdotH * NdotH;
	
    float denom  = (NdotH2 * (a - 1.f) + 1.f);
    denom = PI * denom * denom;
    return a / denom;
}

float calculateGeometryFunctionSchlick(float NdotV, float roughness) {
    float r = (roughness + 1.f);
    float k = (r * r) / 8.f;

    float nom   = NdotV;
    float denom = NdotV * (1.f - k) + k;
	
    return nom / denom;
}

float calculateGeometryFunctionSmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness) {
    float NdotV = doto(normal, viewDirection);
    float NdotL = doto(normal, lightDirection);
    float ggx2  = calculateGeometryFunctionSchlick(NdotV, roughness);
    float ggx1  = calculateGeometryFunctionSchlick(NdotL, roughness);
	
    return ggx1 * ggx2;
}

//Reflectance equation with Cook-Torrance BRDF
vec3 calculateReflectance(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic) {
	
	vec3 dir = lightPosition - fragPosition;
	vec3 lightDirection   = normalize(dir);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float NdotL = doto(normal, lightDirection);     

	float lightDistance = length(dir);
	float attenuation = 1.f / (lightDistance * lightDistance); //Inverse square law
	vec3  radiance = lightDiffuse * attenuation;

	//BRDF
	//float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	//float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(halfwayDirection, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.f) - ks;
    kd *= 1.f - metallic;

	//Lighting equation
	//vec3  nom   = ndf * geo * ks; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	//float denom =  4.f * doto(viewDirection, normal) * NdotL + 0.001f; 
	//vec3  brdf  = nom / denom;

	return (kd * albedo / PI) * radiance * NdotL; //Return only diffuse term
	//return ((kd * albedo / PI + brdf) * radiance) * NdotL; 
}

//Reflectance equation with Cook-Torrance BRDF for directional light
vec3 calculateReflectanceDirectional(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic) {
	
	vec3 dir = -lightPosition;
	vec3 lightDirection   = normalize(dir);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float NdotL = doto(normal, lightDirection);     

	vec3  radiance = lightDiffuse;

	//BRDF
	float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(halfwayDirection, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.f) - ks;
    kd *= 1.f - metallic;

	//Lighting equation
	vec3  nom   = ndf * geo * fres; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	float denom =  4.f * doto(viewDirection, normal) * NdotL + 0.001f; 
	vec3  brdf  = nom / denom;

	return ((kd * albedo / PI + brdf) * radiance) * NdotL; 
}

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo, roughness, metallic);
	float shadow = 1.f - calculatePointLightShadows(index, normal, fragPosition);
	
    return shadow * reflectance;
}

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {
	
	vec3 lightDirection = normalize(light.position - fragPosition);

	//Set intensity depending on if object is inside spotlights halo (or outer halo)
	float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.angle - light.outerAngle);
    float intensity = clamp((theta - light.outerAngle) / epsilon, 0.f, 1.f);

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo, roughness, metallic);

	vec3 coords = vec3(0.f);
	float shadow = 1.f - calculateSpotLightShadows(index, normal, fragPosition, coords);
	float cookie = light.useCookie ? texture(light.cookie, coords.xy).r : 1.f;

    return shadow * reflectance * intensity * cookie;
}

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {
	
	vec3 reflectance = calculateReflectanceDirectional(fragPosition, normal, 
		viewDirection, light.direction, light.diffuse, albedo, roughness, metallic);
	float shadow = 1.f - calculateDirectionalLightShadows(index, normal, fragPosition);
	
    return shadow * reflectance;
}



//Shadows...............................................................................................................................

float random(vec3 seed, int i) {
	vec4 seed4 = vec4(seed, i);
	float dt = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	float sn = mod(dt,3.14);

	return fract(sin(sn) * 43758.5453);
}

//Shadows...................................................................................................................


vec3 sampleDirections3D[20] = vec3[] (
   vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), 
   vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
   vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
   vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
); 

//Poisson disc
vec2 sampleDirections2D[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition) {

	//No shadow
	if(pointLights[i].type == 0)
		return 0.f;
	
	vec4 posLightSpace = vec4(fragPosition - pointLights[i].position, 1);
	vec3 direction = posLightSpace.xyz;
	float curDepth = length(direction) / pointLights[i].farPlane;

	//Dont' draw shadow when too far away from the viewer or from light
	float camDistance = length(fragPosition) / pointLights[i].farPlane;
	if(camDistance > 10.f || curDepth > 1.f)
		return 0.f;
	else {
		float minBias = pointLights[i].bias;
		vec3 dir = normalize(pointLights[i].position - fragPosition);
		float bias = max((minBias * 10.f) * (1.f - abs(dot(norm, dir))), minBias);

		//Hard shadow
		if(pointLights[i].type == 1) {
			float depth = texture(pointLights[i].shadowMap, direction).r;
			return curDepth - bias > depth ? 1.f : 0.f; 
		}

		//Soft shadow

		vec2 size = textureSize(pointLights[i].shadowMap, 0);
		float mag = (size.x + size.y) * (1.f / pow(pointLights[i].scale, 2));
		float dist = length(fragPosition - pointLights[i].position);
		float diskRadius = (1.f + (dist / 150.f)) / mag;

		float testSamples = 4;
		float testShadow = 0;
		for(int j = 0; j < testSamples; j++) {
			float depth = texture(pointLights[i].shadowMap, direction + sampleDirections3D[j] * diskRadius).r;
			testShadow += curDepth - bias > depth ? 1 : 0;        
		}  

		testShadow = testShadow / float(testSamples);

		if(testShadow == 0.f || testShadow == 1.f)
			return testShadow;
		else {
			float shadow = 0.f;
			int samples = pointLights[i].resolution;
			for(int j = 0; j < samples; j++) {
				int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

				float depth = texture(pointLights[i].shadowMap, direction + sampleDirections3D[index] * diskRadius).r;
				shadow += step(depth, curDepth - bias);
			}    

			return (shadow) / float(samples);
		}
	}
}

float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition, inout vec3 coords) {

	vec4 posLightSpace = spotLights[i].lightTransform * vec4(fragPosition, 1.0f);
	coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//No shadow. Called after coordinate computation since these are needed for light cookie regardless
	if(spotLights[i].type == 0.f)
		return 0.f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.f)
        return 0.f;
	else {
		float minBias = spotLights[i].bias;
		vec3 lightDir =  spotLights[i].position - fragPosition;
		float bias = max((minBias * 10.0f) * (1.0f - dot(norm, lightDir)), minBias);
		float curDepth = coords.z - bias;

		//Hard shadow
		if(spotLights[i].type == 1) {
			float depth = texture(spotLights[i].shadowMap, coords.xy).r;
			return curDepth - bias > depth ? 1.f : 0.f; 
		}

		//Soft shadow

		float shadow = 0.f;
		vec2 texelSize = spotLights[i].scale / textureSize(spotLights[i].shadowMap, 0);
		int samples = spotLights[i].resolution;
		for(int j = 0; j < samples; j++) {
			int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

			float depth = texture(spotLights[i].shadowMap, coords.xy + sampleDirections2D[index] * texelSize).r;
			shadow += step(depth, curDepth - bias);
		}    
	
		return shadow / float(samples);
	}
}

float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition) {

	if(directionalLights[i].type == 0)
		return 0.f;

	//TODO: implement hard shadows

	int smIndex = 0;
	float xOffset = 0;
	float yOffset = 0;
	float clipDepth = length(cameraPosition - fragPosition);
	for(int k = 0; k < DIRECTIONAL_SHADOWMAP_COUNT; k++) {
		if(clipDepth < directionalLights[i].cascadeEndClip[k]) {
			xOffset = mod(float(k), 2.f);
			yOffset = floor(float(k) / 2.f);
			smIndex = k;
			break;
		}
	}

	vec4 posLightSpace = directionalLights[i].lightTransforms[smIndex] * vec4(fragPosition, 1.f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//Translate texture coordinates according to current sub shadow map
	coords.x *= 0.5f;
	coords.y *= 0.5f;
	coords.x += xOffset * 0.5f;
	coords.y += yOffset * 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.f)
        return 0.f;
	else {
		float minBias = directionalLights[i].bias;
		float bias = max(minBias * 100.f * (1.f - dot(norm, directionalLights[i].direction)), minBias);
		float curDepth = coords.z - bias;

		float shadow = 0.f;
		vec2 texelSize = 0.8f / textureSize(directionalLights[i].shadowMap, 0);
		int samples = 8;
		for(int j = 0; j < samples; j++) {
			int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

			float depth = texture(directionalLights[i].shadowMap, coords.xy + sampleDirections2D[index] * texelSize).r;
			shadow += step(depth, curDepth - bias);      
		}    
	
		return shadow / float(samples);
	}
}

//Helper functions......................................................................................................................

float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.f);
}

