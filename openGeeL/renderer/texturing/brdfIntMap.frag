#version 430 core

const float PI = 3.14159265359f;

out vec2 color;

in vec2 TexCoords;

float doto(vec3 a, vec3 b);
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

//Generate halfway direction sample with GGX normal distribution function
//http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
vec3 importanceSampleGGX(vec2 samp, vec3 N, float roughness) {
	float e1 = samp.x;
	float e2 = samp.y;

	float a = roughness * roughness;
    float phi = 2.f * PI * e1;
    float cosTheta = sqrt((1.f - e2) / (1.f + (a * a - 1.f) * e2));
    float sinTheta = sqrt(1.f - cosTheta * cosTheta);
	
    vec3 H = vec3(0.f);
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    //Tangent to world space 
    vec3 U = abs(N.z) < 0.999f ? vec3(0.f, 0.f, 1.f) : vec3(1.f, 0.f, 0.f);
    vec3 R = normalize(cross(U, N));
    vec3 F = cross(N, R);
	
    vec3 sampleVec = R * H.x + F * H.y + N * H.z;
    return normalize(sampleVec);
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
		vec3 H = importanceSampleGGX(hammersleySeq(i, sampleCount), N, roughness);
		vec3 L = normalize(2.f * dot(V, H) * H - V);

		//Cheap dot computation since N.z = 1
		float NdotL = max(L.z, 0.f);
        float NdotH = max(H.z, 0.f);
        float VdotH = max(dot(V, H), 0.f);

		if(NdotL > 0.f) {
			//Monte carlo integration with
			//BRDF = (D * G * F) / (4 * NdotL * NdotV)
			//PDF = (D * NdotH) / (4 * VdotH)			//Resulting term: (G * VdotH) / (NdotH * NdotV)
			float brdf = calculateGeometryFunctionSmith(N, V, L, roughness) / (NdotH * NdotV);
			float brdf_VdotH = brdf * VdotH;

			float F = pow(1.f - VdotH, 5.f);
			scale += (1.f - F) * brdf_VdotH;
			bias  += F * brdf_VdotH;
		}
	}

	return vec2(scale / weight, bias / weight);
}


float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0f);
}