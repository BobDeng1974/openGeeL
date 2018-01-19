#version 430 core

#define POSITION_MAP	gPositionDepth
#define NORMAL_MAP		gNormalMet

in vec2 TexCoords;

out float color;

uniform sampler2D POSITION_MAP;
uniform sampler2D NORMAL_MAP;
uniform sampler2D noiseTexture;

#include <shaders/gbufferread.glsl>

uniform vec3 samples[64];
uniform mat4 projection;

uniform float screenWidth;
uniform float screenHeight;
uniform float radius;

vec2 noiseScale() {
	return vec2(screenWidth / 4.f, screenHeight / 4.f);
}

void main() {

	vec3 fragPos = readPosition(TexCoords);
	vec3 normal  = readNormal(TexCoords);

	vec3 random = texture(noiseTexture, TexCoords * noiseScale()).xyz;
	random = normalize(random);

	vec3 tangent   = cross(random, normal);
	vec3 bitangent = cross(normal, tangent); 
	mat3 TBN = mat3(tangent, bitangent, normal);
	
	int sampleCount = 15;
	float occlusion = 0.f;
	for(int i = 0; i < sampleCount; i++) {
		vec3 samp = TBN * samples[i];
		samp = fragPos + samp * radius;

		vec4 offset = vec4(samp, 1.f);
		//Transform to clip space
		offset = projection * offset;
		offset.xyz = offset.xyz / offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float depth = -readDepth(offset.xy);
		//Add to occlusion if depth > samp.z
		occlusion += step(samp.z, depth) * smoothstep(0.f, 1.f, radius / abs(fragPos.z - depth));
	}
	
	color = 1.f - (occlusion / float(sampleCount));
}