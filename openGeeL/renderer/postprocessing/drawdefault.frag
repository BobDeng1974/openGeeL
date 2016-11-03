#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform float exposure;

uniform sampler2D noiseMap;
uniform vec3 noiseScale;


void main() { 

	vec3 hdr = texture(image, TexCoords).rgb;

	//Tone mapping
	vec3 result = vec3(1.0f) - exp(-hdr * exposure);

	//Gamma correction
	result.rgb = pow(result.rgb, vec3(0.4545f));
	
	float dither = texture(noiseMap, TexCoords * noiseScale.xy).r * 0.005f;
	color = vec4(result + dither, 1.0f);
}