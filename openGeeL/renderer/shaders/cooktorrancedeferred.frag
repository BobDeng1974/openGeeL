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
uniform samplerCube irradianceMap;

uniform mat4 inverseView;
uniform vec3 origin;
uniform vec3 ambient;

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

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
float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition);
float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition);

vec3 calculateVolumetricLightColor(vec3 fragPos, vec3 lightPosition, vec3 lightColor, float density);

vec3 calculateIndirectDiffuse(vec3 normal, vec3 kd, vec3 albedo, float occlusion);
vec3 calculateIndirectSpecular(vec3 normal, vec3 view, vec3 albedo, vec3 ks, float roughness, float metallic);


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

	vec3 ks = calculateFresnelTerm(doto(normal, viewDirection), albedo, metallic, roughness);
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic; //metallic surfaces don't refract light => nullify kD if metallic

	gSpecular = 0.f;
	vec3 irradiance = vec3(0.f, 0.f, 0.f);
	for(int i = 0; i < plCount; i++) {
		irradiance += calculatePointLight(i, pointLights[i], normal, fragPosition, viewDirection, albedo, roughness, metallic);
		//irradiance += calculateVolumetricLightColor(fragPosition, pointLights[i].position, pointLights[i].diffuse, 0.001f);
	}
        
	for(int i = 0; i < dlCount; i++) {
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo, roughness, metallic);
		irradiance += calculateVolumetricLightColor(fragPosition, directionalLights[i].direction * -100.f, directionalLights[i].diffuse, 150.f);
	}

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo, roughness, metallic);

	vec3 ambienceDiffuse = calculateIndirectDiffuse(normal, kd, albedo, occlusion); 
	vec3 ambienceSpecular = calculateIndirectSpecular(normal, viewDirection, albedo, ks, roughness, metallic) * (1.f - roughness);

	color = vec4(irradiance + ambienceDiffuse + ambienceSpecular, 1.f);
}

//Lighting.....................................................................................................................................

//Compute fresnel term with Fresnel-Schlick approximation
vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness) {
	vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

	//Simplified term withouth roughness included
    //return F0 + (1.0f - F0) * pow(1.0f - theta, 5.0f);
	vec3 fres = F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - theta, 5.0f);
	return clamp(fres, 0.f, 1.f);
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
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic) {
	
	vec3 dir = lightPosition - fragPosition;
	vec3 lightDirection   = normalize(dir);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float NdotL = doto(normal, lightDirection);     

	float lightDistance = length(dir);
	float attenuation = 1.0f / (lightDistance * lightDistance); //Inverse square law
	vec3  radiance = lightDiffuse * attenuation;

	//BRDF
	float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(halfwayDirection, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic;

	//Lighting equation
	vec3  nom   = ndf * geo * ks; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	float denom =  4.0f * doto(viewDirection, normal) * NdotL + 0.001f; 
	vec3  brdf  = nom / denom;

	gSpecular += luminance(ks) * (1.f - metallic) * attenuation;

	return (((kd * albedo / PI + brdf) * radiance)) * NdotL; 
}

//Reflectance equation with Cook-Torrance BRDF for directional light
vec3 calculateReflectanceDirectional(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic) {
	
	vec3 dir = -lightPosition;
	vec3 lightDirection   = normalize(dir);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float NdotL = doto(normal, lightDirection);     

	float lightDistance = length(dir);
	vec3  radiance = lightDiffuse;

	//BRDF
	float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(halfwayDirection, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic;

	//Lighting equation
	vec3  nom   = ndf * geo * fres; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	float denom =  4.0f * doto(viewDirection, normal) * NdotL + 0.001f; 
	vec3  brdf  = nom / denom;

	gSpecular +=  luminance(ks) * (1.0f - roughness) * NdotL * luminance(radiance);

	return (((kd * albedo / PI + brdf) * radiance)) * NdotL; 
}

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo, roughness, metallic);
	float shadow = 1.0f - calculatePointLightShadows(index, normal, fragPosition);
	
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
	float shadow = 1.0f - calculateSpotLightShadows(index, normal, fragPosition);

    return shadow * reflectance * intensity;
}

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic) {
	
	vec3 reflectance = calculateReflectanceDirectional(fragPosition, normal, 
		viewDirection, light.direction, light.diffuse, albedo, roughness, metallic);
	float shadow = 1.f - calculateDirectionalLightShadows(index, normal, fragPosition);
	
    return shadow * reflectance;
}

vec3 calculateIndirectDiffuse(vec3 normal, vec3 kd, vec3 albedo, float occlusion) {

	vec4 normalWorld = inverseView * vec4(origin + normal, 1.f);
	vec3 irradiance = texture(irradianceMap, normalWorld.xyz).rgb;
	float l = length(irradiance);
	irradiance *= kd;

	//Use ambient color if no 'real' value is read from irradiance map
	float check = step(0.01f, l);
	irradiance = check * irradiance + (1.f - check) * ambient;

	return irradiance * albedo * occlusion;
}


vec3 generateSampledVector(float roughness, float i, float count) {
	float e1 = i / count;
	float e2 = e1 * 0.5f;

	float theta = atan((roughness * sqrt(e1)) / sqrt(1.f - e2));
	float phi   = 2.f * PI * e2;

	return vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}


vec3 calculateIndirectSpecular(vec3 normal, vec3 view, vec3 albedo, vec3 ks, float roughness, float metallic) {

	vec3 normalWorld = normalize((inverseView * vec4(origin + normal, 1.f)).xyz);
	vec3 viewWorld = normalize((inverseView * vec4(origin + view, 1.f)).xyz);
	vec3 reflection = normalize(reflect(-viewWorld, normalWorld));
	vec3 right = cross(reflection, normalWorld);
	vec3 up = cross(reflection, right);

	float NoV = doto(normalWorld, viewWorld);

	vec3 irradiance = vec3(0.f);
	int sampleCount = 10;
	for(int i = 0; i < sampleCount; i++) {
		vec3 sampleVector = generateSampledVector(roughness, i, sampleCount);
		sampleVector = sampleVector.x * right + sampleVector.y * up + sampleVector.z * reflection; //To world coordinates
		sampleVector = normalize(sampleVector);

		vec3 halfway = normalize(sampleVector + viewWorld);
		float cosT = doto(sampleVector, normalWorld);
		float sinT = sqrt(1.f - cosT * cosT);

		float theta = doto(halfway, viewWorld);
		vec3  fresnel = calculateFresnelTerm(theta, albedo, metallic, roughness);
		float geo = calculateGeometryFunctionSmith(normalWorld, viewWorld, sampleVector, roughness);

		ks += fresnel;
		float denom =  1.f / (4.f * NoV * doto(halfway, normalWorld) + 0.001f); 

		irradiance += texture(skybox, sampleVector).rgb * geo * fresnel * sinT * denom;
	}
	
	float samp = 1.f / float(sampleCount);
	ks = ks * samp;
	return irradiance * samp;
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
	if(camDistance > 10.0f || curDepth > 1.f)
		return 0.0f;
	else {
		float minBias = pointLights[i].bias;
		vec3 dir = normalize(pointLights[i].position - fragPosition);
		float bias = max((minBias * 10.f) * (1.f - abs(dot(norm, dir))), minBias);

		vec2 size = textureSize(pointLights[i].shadowMap, 0);
		float mag = (size.x + size.y) * 0.035f;
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
			int samples = 50;
			for(int j = 0; j < samples; j++) {
				int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

				float depth = texture(pointLights[i].shadowMap, direction + sampleDirections3D[index] * diskRadius).r;
				shadow += step(depth, curDepth - bias);
			}    

			return (shadow) / float(samples);
		}
	}
}

float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition) {
	vec4 posLightSpace = spotLights[i].lightTransform * inverseView * vec4(fragPosition, 1.0f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.f)
        return 0.f;
	else {
		float minBias = spotLights[i].bias;
		vec3 lightDir =  spotLights[i].position - fragPosition;
		float bias = max((minBias * 10.0f) * (1.0f - dot(norm, lightDir)), minBias);
		float curDepth = coords.z - bias;

		float shadow = 0.f;
		vec2 texelSize = 1.0f / textureSize(spotLights[i].shadowMap, 0);
		int samples = 8;
		for(int j = 0; j < samples; j++) {
			int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

			float depth = texture(spotLights[i].shadowMap, coords.xy + sampleDirections2D[index] * texelSize).r;
			shadow += step(depth, curDepth - bias);
		}    
	
		return shadow / float(samples);
	}
}

float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition) {
	vec4 posLightSpace = directionalLights[i].lightTransform * inverseView * vec4(fragPosition, 1.0f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.f)
        return 0.f;
	else {
		float mapDepth = texture(directionalLights[i].shadowMap, coords.xy).r;
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
