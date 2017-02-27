#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionDepth;

uniform bool horizontal;
uniform float kernel[5];
uniform float sigma;


float bilateralCoeffient(float baseDepth, float sampleDepth) {
	float diff = abs(baseDepth - sampleDepth);
	float fac = sigma;

	return exp(-(diff * diff) / (2.f * fac * fac));
}


void main() {             

	//Size of single texel
    vec2 texOffset = 1.f / textureSize(image, 0); 
    vec3 base = texture(image, TexCoords).rgb; 
	float baseDepth = -texture(gPositionDepth, TexCoords).z;
	vec3 result = base * kernel[0];

	float hor = step(1.f, float(horizontal));
	float ver = 1 - hor;

	float weights = kernel[0];
	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < 5; i++) {
		vec2 off = offset * i;

		//Check if image borders aren't crossed
		float inBorders = step(TexCoords.x + off.x, 1.f) * 
			step(0.f, TexCoords.x - off.x) * 
			step(TexCoords.y + off.y, 1.f) * 
			step(0.f, TexCoords.y - off.y);

		vec3 samp = texture(image, TexCoords + off).rgb;
		float sampDepth = -texture(gPositionDepth, TexCoords + off).z;
		float weight = bilateralCoeffient(baseDepth, sampDepth) * kernel[i] * inBorders;
		weights += weight;

		//Sample right / top pixel
		result += samp * weight;

		samp = texture(image, TexCoords - off).rgb;
		sampDepth = -texture(gPositionDepth, TexCoords - off).z;
		weight = bilateralCoeffient(baseDepth, sampDepth) * kernel[i] * inBorders;
		weights += weight;

		//Sample left / bottom pixel
		result += samp * weight;
    }

    color = vec4(result / weights, 1.f);
}