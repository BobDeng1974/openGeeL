#version 330 core

in vec2 TexCoords;

out float color;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
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
	vec3 normal  = texture(gNormal, TexCoords).rgb;

	vec3 random = texture(noiseTexture, TexCoords * noiseScale()).xyz;
	random = normalize(random);

	vec3 tangent   = cross(random, normal);
	vec3 bitangent = cross(normal, tangent); 
	mat3 TBN = mat3(tangent, bitangent, normal);

	int sampleCount = 16;
	float occlusion = 0.f;
	for(int i = 0; i < sampleCount; i++) {
		vec3 sample = TBN * samples[i];
		sample = fragPos + sample * radius;

		vec4 offset = vec4(sample, 1.f);
		//Transform to screen space
		offset = projection * offset;
		offset.xyz = offset.xyz / offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float depth = -texture(gPositionDepth, offset.xy).w;	
		
		if(depth >= sample.z)
			occlusion += smoothstep(0.f, 1.f, radius / abs(fragPos.z - depth));;
	}

	color = 1.f - (occlusion / float(sampleCount));
}