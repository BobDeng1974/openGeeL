
#include <shaders/helperfunctions.glsl>

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


vec3 generateSampledVector(float roughness, vec2 samp) {
	float e1 = samp.x;
	float e2 = samp.y;

	float theta = atan((roughness * sqrt(e1)) / sqrt(1.f - e2));
	//float theta = atan((roughness * roughness * sqrt(e1)) / sqrt(1.f - e2));
	float phi   = 2.f * PI * e2;

	return vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}
