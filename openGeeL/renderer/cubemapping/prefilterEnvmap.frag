#version 430 core

const float PI = 3.14159265359f;

out vec4 color;

in vec3 localPos;

uniform float roughness;
uniform samplerCube environmentMap;


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
vec3 importanceSampleGGX(vec2 samp, vec3 N) {
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


void main() {		
	vec3 prefilterColor = vec3(0.f);
    vec3 normal = normalize(localPos);

	int sampleCount = 1024;
	float weight = 0.f;
	for(int i = 0; i < sampleCount; i++) {
		//Generate halfway direction vector and compute reflected direction
		vec3 H = importanceSampleGGX(hammersleySeq(i, sampleCount), normal);
		vec3 L = normalize(2.f * dot(normal, H) * H - normal);

		float NdotL = max(dot(normal, L), 0.f);
		prefilterColor += texture(environmentMap, L).rgb * NdotL;
		weight += NdotL;
	}

	color = vec4(prefilterColor / weight, 1.f);
}