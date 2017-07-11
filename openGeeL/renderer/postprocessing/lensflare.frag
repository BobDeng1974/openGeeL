#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;

uniform float scale;
uniform float samples;
uniform vec3 distortion;

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

    color = vec4(result, 1.f);
}

vec3 sampleChromatic(vec2 TexCoords) {
	vec2 direction = vec2(0.5f);

	return vec3(texture(image, TexCoords + direction * distortion.r).r,
		texture(image, TexCoords + direction * distortion.g).g,
		texture(image, TexCoords + direction * distortion.b).b);

}