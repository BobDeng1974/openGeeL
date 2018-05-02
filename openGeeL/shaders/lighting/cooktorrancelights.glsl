


#include <shaders/lighting/cooktorrance.glsl>
//Note: preferred shadowing shader should be included beforehand (e.g. shadows in view or world space)

vec3 subsurfaceScatteringBack(vec3 fragPosition, vec3 normal, vec3 viewDirection, vec4 albedo, vec3 lightPosition, 
	vec3 lightDiffuse, float roughness, float metallic, float travelDistance) {

	vec3 backsideReflectance = calculateReflectance(fragPosition, -normal, 
		viewDirection, lightPosition, lightDiffuse, albedo.rgb, roughness, metallic);

	//Approximate transmittance coefficient via albedo alpha channel
	float transCoef = 1.f / (pow(1.f - albedo.a, 5.f) + 0.000001f);

	return backsideReflectance * exp(-travelDistance * transCoef);
}


#if (VOLUMETRIC_LIGHT == 1)

vec3 calculateVolumetricLightColor(vec3 fragPos, vec3 lightPosition, vec3 lightColor, float strength, float density) {
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
	float attenuation = 1.f / pow(dist, density); // (dist * dist);

	return lightColor * attenuation * strength * lightInView;
}

#endif


vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec4 albedo, float roughness, float metallic) {

	vec3 reflectance = calculateReflectance(fragPosition, normal, 
		viewDirection, light.position, light.diffuse, albedo.rgb, roughness, metallic);

#if (VOLUMETRIC_LIGHT == 1)
	vec3 volumetricColor = calculateVolumetricLightColor(fragPosition, light.position, light.diffuse, 
		light.volumetricStrength, light.volumetricDensity);
#endif

	float travel = 0.f;
	float shadow = 1.f - light.shadowIntensity * calculatePointLightShadows(index, normal, fragPosition, travel);

#if (BACKFACE_SUBSURFACE_SCATTERING == 1)
	vec3 sss = subsurfaceScatteringBack(fragPosition, normal, viewDirection, albedo, 
		light.position, light.diffuse, roughness, metallic, travel);

#if (VOLUMETRIC_LIGHT == 1)
	return shadow * reflectance + sss + volumetricColor;
#else
	return shadow * reflectance + sss;
#endif
	
#else

#if (VOLUMETRIC_LIGHT == 1)
	return shadow * reflectance + volumetricColor;
#else
	return shadow * reflectance;
#endif

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
	inout vec3 diffuse, inout vec3 specular) {

	vec3 tempDiff = vec3(0.f);
	vec3 tempSpec = vec3(0.f);

	calculateReflectance(fragPosition, normal, viewDirection, light.position, light.diffuse, 
		albedo.rgb, roughness, metallic, tempDiff, tempSpec);

#if (VOLUMETRIC_LIGHT == 1)
	vec3 volumetricColor = calculateVolumetricLightColor(fragPosition, light.position, light.diffuse, 
		light.volumetricStrength, light.volumetricDensity);
#endif

	float travel = 0.f;
	float shadow = 1.f - light.shadowIntensity * calculatePointLightShadows(index, normal, fragPosition, travel);

#if (BACKFACE_SUBSURFACE_SCATTERING == 1)
	vec3 sss = subsurfaceScatteringBack(fragPosition, normal, viewDirection, albedo, 
		light.position, light.diffuse, roughness, metallic, travel);

#if (VOLUMETRIC_LIGHT == 1)
	diffuse  += tempDiff * shadow + sss + volumetricColor;
#else
	diffuse  += tempDiff * shadow + sss;
#endif
	specular += tempSpec * shadow;

#else

#if (VOLUMETRIC_LIGHT == 1)
	diffuse  += tempDiff * shadow + volumetricColor;
#else
	diffuse  += tempDiff * shadow;
#endif
	specular += tempSpec * shadow;

#endif
}

void calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float roughness, float metallic, 
	inout vec3 diffuse, inout vec3 specular) {
	
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
	inout vec3 diffuse, inout vec3 specular) {
	
	vec3 tempDiff = vec3(0.f);
	vec3 tempSpec = vec3(0.f);

	calculateReflectanceDirectional(fragPosition, normal, viewDirection, light.direction, light.diffuse, 
		albedo, roughness, metallic, tempDiff, tempSpec);
	float shadow = 1.f - light.shadowIntensity * calculateDirectionalLightShadows(index, normal, fragPosition);

	diffuse  += tempDiff * shadow;
	specular += tempSpec * shadow;
}


#endif