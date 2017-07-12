#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D brightnessFilter;
uniform sampler2D dirt;
uniform sampler2D starburst;

uniform bool useDirt;
uniform bool useStarburst;

uniform float strength;
uniform float scale;
uniform float samples;
uniform vec3 distortion;

uniform mat3 starTransform;

vec3 sampleChromatic(vec2 TexCoords);

void main() {       
	const float baseLength = length(vec2(0.5f));      
	
	vec2 coords = 1.f - TexCoords;
	vec2 ghostDir = (0.5f - coords) * scale;

	vec3 result = vec3(0.f);
    for (float i = 0.f; i <= samples; i += 1.f) { 
        vec2 offset = fract(coords + i * ghostDir);
  
		float weight = length(0.5f - offset) / baseLength;
		weight = pow(1.f - weight, 5.f);

        result += sampleChromatic(offset) * weight;
    }

	vec3 baseC = texture(image, TexCoords).rgb;

	vec2 rotTexCoords = (starTransform * vec3(TexCoords, 1.f)).xy;
	vec3 star  = useStarburst ? texture(starburst, rotTexCoords).rgb : vec3(1.f);
	vec3 dirty = useDirt ? texture(dirt, TexCoords).rgb : vec3(1.f);

    color = vec4(baseC + result * strength * dirty * star, 1.f);
}

vec3 sampleChromatic(vec2 TexCoords) {
	vec2 direction = vec2(0.5f);

	return vec3(texture(brightnessFilter, TexCoords + direction * distortion.r).r,
		texture(brightnessFilter, TexCoords + direction * distortion.g).g,
		texture(brightnessFilter, TexCoords + direction * distortion.b).b);

}