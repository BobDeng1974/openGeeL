#version 430 core

in vec2 TexCoords;

out vec4 color;

const int kernelSize = 3;

uniform float weights[kernelSize];
uniform float offsets[kernelSize];

uniform sampler2D image;
uniform sampler2D gPositionDepth;

uniform bool horizontal;
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
	vec3 result = base * weights[0];

	float hor = step(1.f, float(horizontal));
	float ver = 1 - hor;

	float weightSum = weights[0];
	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < kernelSize; i++) {
		vec2 off = offset * offsets[i];

		//Check if image borders aren't crossed
		float inBorders = step(TexCoords.x + off.x, 1.f) * 
			step(0.f, TexCoords.x - off.x) * 
			step(TexCoords.y + off.y, 1.f) * 
			step(0.f, TexCoords.y - off.y);

		vec3 samp = texture(image, TexCoords + off).rgb;
		float sampDepth = -texture(gPositionDepth, TexCoords + off).z;
		float weight = bilateralCoeffient(baseDepth, sampDepth) * weights[i] * inBorders;
		weightSum += weight;

		//Sample right / top pixel
		result += samp * weight;

		samp = texture(image, TexCoords - off).rgb;
		sampDepth = -texture(gPositionDepth, TexCoords - off).z;
		weight = bilateralCoeffient(baseDepth, sampDepth) * weights[i] * inBorders;
		weightSum += weight;

		//Sample left / bottom pixel
		result += samp * weight;
    }

    color = vec4(result / weightSum, 1.f);
}