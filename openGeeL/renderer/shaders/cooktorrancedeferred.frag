#version 430

const float PI = 3.14159265359;

struct PointLight {
	samplerCube shadowMap;

    vec3 position;
    vec3 diffuse;

	float bias;
	float farPlane;
};

struct SpotLight {
	sampler2D shadowMap;
	mat4 lightTransform;

    vec3 position;
    vec3 direction;
	vec3 diffuse;

    float angle;
    float outerAngle;
	float bias;
};


struct DirectionalLight {
	sampler2D shadowMap;
	mat4 lightTransform;

	vec3 direction;
    vec3 diffuse;

	float bias;
};

in vec2 textureCoordinates;
in mat3 TBN;

layout (location = 0) out vec4 color;
layout (location = 1) out float gSpecular;
  
uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuseSpec;

uniform sampler2D ssao;
uniform int useSSAO;

uniform samplerCube skybox;

uniform mat4 inverseView;
uniform vec3 origin;
uniform vec3 ambient;

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, vec3 kd, vec3 ks, float roughness, vec3 reflection);

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, vec3 kd, vec3 ks, float roughness, vec3 reflection);

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, vec3 kd, vec3 ks, float roughness, vec3 reflection);

vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness);

float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition);
float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition);
float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition);

//Return dot(a,b) >= 0
float doto(vec3 a, vec3 b);
float random(vec3 seed, int i);
float luminance(vec3 color);

void main() {
	vec3 fragPosition = texture(gPositionDepth, textureCoordinates).rgb;
    vec3 normal		  = texture(gNormalMet, textureCoordinates).rgb;
    vec3 albedo		  = texture(gDiffuseSpec, textureCoordinates).rgb;

	float roughness	  = texture(gDiffuseSpec, textureCoordinates).a;
	float metallic    = texture(gNormalMet, textureCoordinates).a;
	float occlusion   = (useSSAO == 1) ? texture(ssao, textureCoordinates).r : 1.f;

	vec3  viewDirection = normalize(-fragPosition);
	float theta = doto(normal, viewDirection);
	vec3  fresnel = calculateFresnelTerm(theta, albedo, metallic, roughness);

	vec3 ks = fresnel;
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic; //metallic surfaces don't refract light => nullify kD if metallic

	vec4 reflectionDir = inverseView * vec4(origin + reflect(fragPosition, normal), 1.f);
	vec3 reflection = texture(skybox, reflectionDir.rgb).rgb * ks * (1.0f - roughness);

	gSpecular = 0.f;
	vec3 irradiance = vec3(0.f, 0.f, 0.f);
	for(int i = 0; i < plCount; i++)
        irradiance += calculatePointLight(i, pointLights[i], normal, fragPosition, viewDirection, albedo, kd, ks, roughness, reflection);

	for(int i = 0; i < dlCount; i++)
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo, kd, ks, roughness, reflection);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo, kd, ks, roughness, reflection);

	vec3 ambience = ambient * albedo * occlusion;

	color = vec4(irradiance + ambience, 1.f);
}

//Lighting.....................................................................................................................................

//Compute fresnel term with Fresnel-Schlick approximation
vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness) {
	vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

	//Simplified term withouth roughness included
    //return F0 + (1.0f - F0) * pow(1.0f - theta, 5.0f);
	return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - theta, 5.0f);
}

//Trowbridge-Reitz GGX normal distribution function
float calculateNormalDistrubution(vec3 normal, vec3 halfway, float roughness) {
    float a = roughness * roughness;
    float NdotH  = doto(normal, halfway);
    float NdotH2 = NdotH * NdotH;
	
    float denom  = (NdotH2 * (a - 1.0f) + 1.0f);
    denom = PI * denom * denom;
    return a / denom;
}

float calculateGeometryFunctionSchlick(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float nom   = NdotV;
    float denom = NdotV * (1.0f - k) + k;
	
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
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, vec3 kd, vec3 ks, float roughness, bool directional, vec3 reflection) {
	
	vec3 dir = directional ? -lightPosition : lightPosition - fragPosition;
	vec3 lightDirection   = normalize(dir);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);

	float lightDistance = length(dir);
	float attenuation = directional ? 1.0f : (1.0f / (lightDistance * lightDistance)); //Inverse square law
	vec3  radiance = lightDiffuse * attenuation;

	//BRDF
	float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);

	vec3  nom   = ndf * geo * ks; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	float denom =  4.0f * doto(viewDirection, normal) * doto(lightDirection, normal) + 0.001f; 
	vec3  brdf  = nom / denom;

	//Lighting equation
	float NdotL = doto(normal, lightDirection);     
	
	gSpecular +=  luminance(ks) * (1.0f - roughness) * NdotL * luminance(radiance);

	return (((kd * albedo / PI + brdf) * radiance) + reflection) * NdotL; 
}

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, vec3 kd, vec3 ks, float roughness, vec3 reflection) {

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo, kd, ks, roughness, false, reflection);
	float shadow = 1.0f - calculatePointLightShadows(index, normal, fragPosition);
	
    return shadow * reflectance;
}

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, vec3 kd, vec3 ks, float roughness, vec3 reflection) {
	
	vec3 lightDirection = normalize(light.position - fragPosition);

	//Set intensity depending on if object is inside spotlights halo (or outer halo)
	float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.angle - light.outerAngle);
    float intensity = clamp((theta - light.outerAngle) / epsilon, 0.0, 1.0);

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo, kd, ks, roughness, false, reflection);
	float shadow = 1.0f - calculateSpotLightShadows(index, normal, fragPosition);

    return shadow * reflectance * intensity;
}

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, vec3 kd, vec3 ks, float roughness, vec3 reflection) {
	
	vec3 reflectance = calculateReflectance(fragPosition, normal, 
		viewDirection, light.direction, light.diffuse, albedo, kd, ks, roughness, true, reflection);
	float shadow = 1.0f - calculateDirectionalLightShadows(index, normal, fragPosition);
	
    return shadow * reflectance;
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

	vec4 posLightSpace = inverseView * vec4(origin + fragPosition - pointLights[i].position, 1);
	vec3 direction = posLightSpace.xyz;
	float curDepth = length(direction) / pointLights[i].farPlane;

	//Dont' draw shadow when too far away from the viewer or from light
	float camDistance = length(fragPosition) / pointLights[i].farPlane;
	if(camDistance > 1.0f || curDepth > 0.5f)
		return 0.0f;

	//float minBias = pointLights[i].bias;
	float minBias = 0.006f;
	vec3 lightDir =  spotLights[i].position - fragPosition;
	float bias = max((minBias * 5.0f) * (1.0f - dot(norm, lightDir)), minBias);

	float shadow = 0.0f;
	int samples = 5;
	float diskRadius = 0.03f;

	for(int j = 0; j < samples; j++) {
		int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

		float depth = texture(pointLights[i].shadowMap, direction + sampleDirections3D[index] * diskRadius).r;
		shadow += curDepth - bias > depth ? 1.0f : 0.0f;        
	}    

	return shadow / float(samples);
}

float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition) {
	vec4 posLightSpace = spotLights[i].lightTransform * inverseView * vec4(fragPosition, 1.0f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.0f)
        return 0.0f;
	
	//float minBias = spotLights[i].bias;
	float minBias = 0.001f;
	vec3 lightDir =  spotLights[i].position - fragPosition;
	float bias = max((minBias * 10.0f) * (1.0f - dot(norm, lightDir)), minBias);
	float curDepth = coords.z - bias;

	float shadow = 0.0;
	vec2 texelSize = 1.0f / textureSize(spotLights[i].shadowMap, 0);
	//Interpolate shadow map in kernel around point
	int kernel = 1;
	for(int x = -kernel; x <= kernel; x++) {
		for(int y = -kernel; y <= kernel; y++) {
			float depth = texture(spotLights[i].shadowMap, coords.xy + vec2(x, y) * texelSize).r;
			shadow += curDepth - bias > depth ? 1.0f : 0.0f;     
		}    
	}
	
	int kernelSize = 2 * kernel + 1; 
	return shadow / (kernelSize * kernelSize);
}

float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition) {
	vec4 posLightSpace = directionalLights[i].lightTransform * inverseView * vec4(fragPosition, 1.0f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.0f)
        return 0.0f;

	float mapDepth = texture(directionalLights[i].shadowMap, coords.xy).r;
	float bias = directionalLights[i].bias;
	//float bias = max(0.05f * (1.0f - dot(norm, lightDir)), 0.005f);
	float curDepth = coords.z - bias;

	float shadow = 0.0;
	vec2 texelSize = 1.0f / textureSize(directionalLights[i].shadowMap, 0);
	//Interpolate shadow map in kernel around point
	int kernel = 1;
	for(int x = -kernel; x <= kernel; x++) {
		for(int y = -kernel; y <= kernel; y++) {
			float depth = texture(directionalLights[i].shadowMap, coords.xy + vec2(x, y) * texelSize).r; 
			shadow += curDepth - bias > depth ? 1.0f : 0.0f;        
		}    
	}
	
	int kernelSize = 2 * kernel + 1; 
	return shadow / (kernelSize * kernelSize);
}


//Helper functions......................................................................................................................

float luminance(vec3 color) {
	return (0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b);
}

float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0f);
}

float random(vec3 seed, int i) {
	vec4 seed4 = vec4(seed, i);
	float dt = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	float sn = mod(dt,3.14);

	return fract(sin(sn) * 43758.5453);
}
