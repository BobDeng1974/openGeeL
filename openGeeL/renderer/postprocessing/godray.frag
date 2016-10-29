#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 lightPosition;
uniform int samples;


float random(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() { 

	float falloff = pow(TexCoords.y, 3.5f);
	float density = 2.0f;
	float decay = 1.0f;
	float weight = 1.0f / float(samples);

	vec2 coords = TexCoords;
	vec2 deltaCoords = TexCoords - lightPosition.xy;
	deltaCoords *= 1.0f / (float(samples) * density);
	
	float currDecay = 1.0f;
	vec3 result = texture(image, TexCoords).rgb;

	float i = 0;
	while(i < samples) {
		float offset = 0.5f + 0.5f * random(coords);
		coords -= deltaCoords * offset;
		result += texture(image, coords).rgb * weight * currDecay * offset * falloff;
		currDecay *= decay;

		i += offset;
	}

	color = vec4(result, 1.0f);
}