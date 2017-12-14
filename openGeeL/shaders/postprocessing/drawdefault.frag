#version 430 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform float exposure;

uniform sampler2D noiseMap;
uniform vec3 noiseScale;


//0 => Simple Reinhardt 1
//1 => Simple Reinhardt 2
//2 => Hejl/Burgess-Dawson
//3 => Uncharted 2
#define TONEMAPPING_METHOD 3


#if (TONEMAPPING_METHOD == 3)
float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x) {
   return ((x * (A * x + C * B) + D * E) / (x * (A * x + B)+ D * F)) - E / F;
}

float exposureBias = 2.f;
vec3 whiteScale = 1.f / Uncharted2Tonemap(vec3(W));
#endif



void main() { 
	vec3 imageColor = texture(image, TexCoords).rgb;
	imageColor *= exposure;

#if (TONEMAPPING_METHOD == 0)
	vec3 result = vec3(1.f) - exp(-imageColor);
	result = pow(result.rgb, vec3(0.4545f));

#elif (TONEMAPPING_METHOD == 1)
	vec3 result = result / (1.f + result);
	result = pow(result.rgb, vec3(0.4545f));
	
#elif (TONEMAPPING_METHOD == 2)
	vec3 result = max(vec3(0.f), imageColor - vec3(0.004f));
	 result = (result * (6.2f * result + 0.5f)) / (result * (6.2f * result + 1.7f) + 0.06f);

#elif (TONEMAPPING_METHOD == 3)
	vec3 result = Uncharted2Tonemap(exposureBias * imageColor) * whiteScale;
	result = pow(result.rgb, vec3(0.4545f));

#endif

	float dither = texture(noiseMap, TexCoords * noiseScale.xy).r * 0.003f;
	color = vec4(result + dither, 1.f);
}