#version 430 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 lightPosition;
uniform vec3 lightPositionView;
uniform int samples;
uniform int raysOnly;

precision highp float;
float random(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() { 

	float angle = max(0, dot(lightPositionView, vec3(0.f, 0.f, 1.f)) - 0.25);
	float yFalloff = pow(TexCoords.y, 3.f);
	float density = 2.0f;
	float decay = 1.0f;
	float weight = 1.0f / float(samples);
	float falloff = angle * yFalloff * weight;

	vec2 coords = TexCoords;
	vec2 deltaCoords = TexCoords - lightPosition.xy;
	deltaCoords *= 1.0f / (float(samples) * density);
	
	float currDecay = 1.0f;
	vec3 result = step(raysOnly, 0.f) * texture(image, TexCoords).rgb;

	float offset = random(coords);
	for(int i = 0; i < samples; i++) {
		coords = TexCoords - deltaCoords * offset;
		result += texture(image, coords).rgb * currDecay * falloff;
		currDecay *= decay;

		offset ++;
	}
	
	/*
	float i = 0;
	while(i < samples) {
		float offset = 0.5f + step(0.1f, angle) * 0.5f * max(0.1f, random(coords));
		coords -= deltaCoords * offset;
		result += texture(image, coords).rgb * currDecay * falloff;
		currDecay *= decay;

		i += offset;
	}*/
	
	color = vec4(result, 1.0f);
}