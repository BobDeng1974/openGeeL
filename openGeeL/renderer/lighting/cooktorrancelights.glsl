

#include <renderer/lighting/cooktorrance.glsl>
//Note: preferred shadowing shader should be included beforehand (e.g. shadows in view or world space)

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

#if (SUBSURFACE_SCATTERING == 1)
	vec3 sss = subsurfaceScatteringBack(fragPosition, normal, viewDirection, albedo, 
		light.position, light.diffuse, roughness, metallic, travel);

	return shadow * reflectance + sss;
#else
	return shadow * reflectance;
#endif
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

#if (DIFFUSE_SPECULAR_SEPARATION == 1)

void calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec4 albedo, float roughness, float metallic, 
	out vec3 diffuse, out vec3 specular) {

	vec3 tempDiff = vec3(0.f);
	vec3 tempSpec = vec3(0.f);

	calculateReflectance(fragPosition, normal, viewDirection, light.position, light.diffuse, 
		albedo.rgb, roughness, metallic, tempDiff, tempSpec);

	float travel = 0.f;
	float shadow = 1.f - light.shadowIntensity * calculatePointLightShadows(index, normal, fragPosition, travel);

#if (SUBSURFACE_SCATTERING == 1)
	vec3 sss = subsurfaceScatteringBack(fragPosition, normal, viewDirection, albedo, 
		light.position, light.diffuse, roughness, metallic, travel);

	diffuse  += tempDiff * shadow + sss;
	specular += tempSpec * shadow;
#else
	diffuse  += tempDiff * shadow;
	specular += tempSpec * shadow;
#endif
}

void calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic, 
	out vec3 diffuse, out vec3 specular) {
	
	vec3 lightDirection = normalize(light.position - fragPosition);

	//Set intensity depending on if object is inside spotlights halo (or outer halo)
	float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.angle - light.outerAngle);
    float intensity = clamp((theta - light.outerAngle) / epsilon, 0.f, 1.f);

	vec3 tempDiff = vec3(0.f);
	vec3 tempSpec = vec3(0.f);

	calculateReflectance(fragPosition, normal, viewDirection, light.position, light.diffuse, 
		albedo, roughness, metallic, tempDiff, tempSpec);

	vec3 coords = vec3(0.f);
	float shadow = 1.f - light.shadowIntensity * calculateSpotLightShadows(index, normal, fragPosition, coords);
	float cookie = light.useCookie ? texture(light.cookie, coords.xy).r : 1.f;

	float fac = shadow * intensity * cookie;

	diffuse  += tempDiff * fac;
	specular += tempSpec * fac;
}

void calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic,
	out vec3 diffuse, out vec3 specular) {
	
	vec3 tempDiff = vec3(0.f);
	vec3 tempSpec = vec3(0.f);

	calculateReflectanceDirectional(fragPosition, normal, viewDirection, light.direction, light.diffuse, 
		albedo, roughness, metallic, tempDiff, tempSpec);
	float shadow = 1.f - calculateDirectionalLightShadows(index, normal, fragPosition);

	diffuse  += tempDiff * shadow;
	specular += tempSpec * shadow;
}


#endif