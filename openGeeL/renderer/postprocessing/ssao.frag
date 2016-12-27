#version 430 core

in vec2 TexCoords;

out float color;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;
uniform sampler2D noiseTexture;

uniform vec3 samples[64];
uniform mat4 projection;

uniform float screenWidth;
uniform float screenHeight;
uniform float radius;

vec2 noiseScale() {
	return vec2(screenWidth / 4.f, screenHeight / 4.f);
}

void main() {

	vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
	vec3 normal  = texture(gNormalMet, TexCoords).rgb;

	vec3 random = texture(noiseTexture, TexCoords * noiseScale()).xyz;
	random = normalize(random);

	vec3 tangent   = cross(random, normal);
	vec3 bitangent = cross(normal, tangent); 
	mat3 TBN = mat3(tangent, bitangent, normal);

	int sampleCount = 20;
	float occlusion = 0.f;
	for(int i = 0; i < sampleCount; i++) {
		vec3 samp = TBN * samples[i];
		samp = fragPos + samp * radius;

		vec4 offset = vec4(samp, 1.f);
		//Transform to clip space
		offset = projection * offset;
		offset.xyz = offset.xyz / offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float depth = texture(gPositionDepth, offset.xy).z;
		if(depth > samp.z)
			occlusion += smoothstep(0.f, 1.f, radius / abs(fragPos.z - depth));
	}

	color = 1.f - (occlusion / float(sampleCount));
}