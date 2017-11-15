

#define DIFFUSE_SPECULAR_SEPARATION 1

//Compute fresnel term with Fresnel-Schlick approximation
vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness) {
	vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

	//Simplified term withouth roughness included
    //return F0 + (1.f - F0) * pow(1.f - theta, 5.f);
	vec3 fres = F0 + (max(vec3(1.f - roughness), F0) - F0) * pow(1.f - theta, 5.f);
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
	float attenuation = 1.f / (lightDistance * lightDistance); //Inverse square law
	vec3  radiance = lightDiffuse * attenuation;

	//BRDF
	float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(normal, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.f) - ks;
    kd *= 1.f - metallic;

	//Lighting equation
	vec3  nom   = ndf * geo * ks; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	float denom =  4.f * doto(normal, viewDirection) * NdotL + 0.001f; 
	vec3  brdf  = nom / denom;

	return ((kd * albedo / PI + brdf) * radiance) * NdotL; 
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
	vec3 fres = calculateFresnelTerm(doto(normal, viewDirection), albedo, metallic, roughness);

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

#if (DIFFUSE_SPECULAR_SEPARATION == 1)

//Reflectance equation with Cook-Torrance BRDF
void calculateReflectance(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic, 
	out vec3 diffuse, out vec3 specular) {
	
	vec3 dir = lightPosition - fragPosition;
	vec3 lightDirection   = normalize(dir);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float NdotL = doto(normal, lightDirection);     

	float lightDistance = length(dir);
	float attenuation = 1.f / (lightDistance * lightDistance); //Inverse square law
	vec3  radiance = lightDiffuse * attenuation;

	//BRDF
	float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(normal, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.f) - ks;
    kd *= 1.f - metallic;

	//Lighting equation
	vec3  nom   = ndf * geo * ks; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	float denom =  4.f * doto(normal, viewDirection) * NdotL + 0.001f; 
	vec3  brdf  = nom / denom;

	diffuse  = ((kd * albedo / PI) * radiance) * NdotL; 
	specular = brdf * radiance * NdotL; 
}


//Reflectance equation with Cook-Torrance BRDF for directional light
void calculateReflectanceDirectional(vec3 fragPosition, vec3 normal, vec3 viewDirection, 
	vec3 lightPosition, vec3 lightDiffuse, vec3 albedo, float roughness, float metallic,
	out vec3 diffuse, out vec3 specular) {
	
	vec3 dir = -lightPosition;
	vec3 lightDirection   = normalize(dir);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float NdotL = doto(normal, lightDirection);     

	vec3  radiance = lightDiffuse;

	//BRDF
	float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, lightDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(normal, viewDirection), albedo, metallic, roughness);

	vec3 ks = fres;
    vec3 kd = vec3(1.f) - ks;
    kd *= 1.f - metallic;

	//Lighting equation
	vec3  nom   = ndf * geo * fres; //Fresnel term equivalent to kS
	//add small fraction to prevent ill behaviour when dividing by zero (shadows no longer additive)
	float denom =  4.f * doto(viewDirection, normal) * NdotL + 0.001f; 
	vec3  brdf  = nom / denom;

	diffuse  = ((kd * albedo / PI) * radiance) * NdotL; 
	specular = brdf * radiance * NdotL;
}


#endif