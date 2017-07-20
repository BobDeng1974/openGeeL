#version 430

#include <renderer/shaders/helperfunctions.glsl>
#include <renderer/shaders/sampling.glsl>
#include <renderer/lights/lights.glsl>
#include <renderer/lighting/cooktorrance.glsl>


in vec2 textureCoordinates;
in mat3 TBN;

layout (location = 0) out vec4 color;
  
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


//Back-lit subsurface scattering using Beer-Lambert law for transmittance
vec3 subsurfaceScatteringBack(vec3 fragPosition, vec3 normal, vec3 viewDirection, vec4 albedo, vec3 lightPosition, 
	vec3 lightDiffuse, float roughness, float metallic, float travelDistance);

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec4 albedo, float roughness, float metallic);

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic);

vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness);

float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition, inout float travelDistance);
float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition, inout vec3 coords);
float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition);

vec3 calculateVolumetricLightColor(vec3 fragPos, vec3 lightPosition, vec3 lightColor, float density);
float luminance(vec3 color);


void main() {
	vec4 posRough = texture(gPositionRoughness, textureCoordinates);
	vec3 fragPosition = posRough.rgb;

	//Discard pixel if it is not connected to any position in scene (Will be rendered black anyway)
	discard(length(fragPosition) <= 0.001f);

	vec4 normMet  = texture(gNormalMet, textureCoordinates);

    vec3 normal		= normMet.rgb;
    vec4 albedo		= texture(gDiffuse, textureCoordinates);
	vec3 emissivity = useEmissivity ? texture(gEmissivity, textureCoordinates).rgb : vec3(0.f);

	float roughness	= posRough.a;
	float metallic  = normMet.a;

	vec3  viewDirection = normalize(-fragPosition);


	vec3 irradiance = albedo.rgb * emissivity;
	for(int i = 0; i < plCount; i++) {
		irradiance += calculatePointLight(i, pointLights[i], normal, fragPosition, viewDirection, albedo, roughness, metallic);
		//irradiance += calculateVolumetricLightColor(fragPosition, pointLights[i].position, pointLights[i].diffuse, 0.001f);
	}
       
	for(int i = 0; i < dlCount; i++) {
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic);
		irradiance += calculateVolumetricLightColor(fragPosition, directionalLights[i].direction * -100.f, directionalLights[i].diffuse, 150.f);
	}

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic);

	color = vec4(irradiance, 1.f);
}

//Lighting.....................................................................................................................................


vec3 subsurfaceScatteringBack(vec3 fragPosition, vec3 normal, vec3 viewDirection, vec4 albedo, vec3 lightPosition, 
	vec3 lightDiffuse, float roughness, float metallic, float travelDistance) {

	vec3 backsideReflectance = calculateReflectance(fragPosition, -normal, 
		viewDirection, lightPosition, lightDiffuse, albedo.rgb, roughness, metallic);

	//Approximate transmittance coefficient via albedo alpha channel
	float transCoef = 1.f / (pow(1.f - albedo.a, 5.f) + 0.000001f);

	return backsideReflectance * exp(-travelDistance * transCoef);
}


vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec4 albedo, float roughness, float metallic) {

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo.rgb, roughness, metallic);

	float travel = 0.f;
	float shadow = 1.f - light.shadowIntensity * calculatePointLightShadows(index, normal, fragPosition, travel);
	
	//vec3 sss = subsurfaceScatteringBack(fragPosition, normal, viewDirection, albedo, 
	//	light.position, light.diffuse, roughness, metallic, travel);

	 //return shadow * reflectance + sss;
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
	float shadow = 1.f - light.shadowIntensity * calculateSpotLightShadows(index, normal, fragPosition, coords);
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


//Shadows...................................................................................................................

float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition, inout float travelDistance) {

	//No shadow
	if(pointLights[i].type == 0)
		return 0.f;
	
	vec4 posLightSpace = inverseView * vec4(origin + fragPosition - pointLights[i].position, 1);
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

			travelDistance = abs(curDepth - depth) + bias;
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

			travelDistance += abs(curDepth - depth);
			testShadow += curDepth - bias > depth ? 1 : 0;        
		}  

		travelDistance = travelDistance / float(testSamples) + bias;
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

	vec4 posLightSpace = spotLights[i].lightTransform * inverseView * vec4(fragPosition, 1.0f);
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
	float clipDepth = (projection * vec4(fragPosition, 1.f)).z;
	for(int k = 0; k < DIRECTIONAL_SHADOWMAP_COUNT; k++) {
		if(clipDepth < directionalLights[i].cascadeEndClip[k]) {
			xOffset = mod(float(k), 2.f);
			yOffset = floor(float(k) / 2.f);
			smIndex = k;
			break;
		}
	}

	vec4 posLightSpace = directionalLights[i].lightTransforms[smIndex] * inverseView * vec4(fragPosition, 1.f);
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
