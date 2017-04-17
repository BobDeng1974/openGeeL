#version 430 core

const float PI = 3.14159265359f;

out vec2 color;

in vec2 TexCoords;


vec2 integrateBRDF(float NdotV, float roughness);

void main() {
	color = integrateBRDF(TexCoords.x, TexCoords.y);
}


float calculateGeometryFunctionSchlick(float NdotV, float roughness) {
    float k = (roughness * roughness) / 2.0f;
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



vec2 integrateBRDF(float NdotV, float roughness) {

	vec3 V = vec3(sqrt(1.f - NdotV * NdotV), 0.f, NdotV);
    vec3 N = vec3(0.f, 0.f, 1.f);

	float scale = 0.f;
	float bias  = 0.f;

	int sampleCount = 1024;
	float weight = float(sampleCount);
	for(int i = 0; i < sampleCount; i++) {
		//Generate halfway direction vector and compute reflected direction
		vec3 H = importanceSampleGGX(hammersleySeq(i, sampleCount), N);
		vec3 L = normalize(2.f * dot(V, H) * H - V);

		//Cheap dot computation since N.z = 1
		float NdotL = max(L.z, 0.f);
        float NdotH = max(H.z, 0.f);
        float VdotH = max(dot(V, H), 0.f);

		//Monte carlo integration with
		//BRDF = (D * G * F) / (4 * NdotL * NdotV)
		//PDF = (D * NdotH) / (4 * VdotH)		//Resulting term: (G * VdotH) / (NdotH * NdotV)
		float brdf = calculateGeometryFunctionSmith(N, V, L, roughness) / (NdotH * NdotV);
		float brdf_VdotH = brdf * VdotH;

		float F = pow(1.f - VdotH, 5.f);
		scale += (1.f - F) * brdf_VdotH;
		bias  += F * brdf_VdotH;
	}

	return vec2(scale / weight, bias / weight);
}