#version 430 core

in vec2 TexCoords;

out vec4 color;

const int kernelSize = 3;

uniform float weights[kernelSize];
uniform float offsets[kernelSize];

uniform sampler2D image;
uniform bool horizontal;
uniform float sigma;


float bilateralCoeffient(vec3 baseColor, vec3 sampleColor) {
	float diff = distance(baseColor, sampleColor);
	float fac = sigma;

	return exp(-(diff * diff) / (2.f * fac * fac));
}


void main() {             

	//Size of single texel
    vec2 texOffset = 1.f / textureSize(image, 0); 
    vec3 base = texture(image, TexCoords).rgb; 
	vec3 result = base * weights[0];

	float hor = step(1.f, float(horizontal));
	float ver = 1 - hor;

	float weightSum = weights[0];
	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < kernelSize; i++) {
		vec2 off = offset * i;

		//Check if image borders aren't crossed
		float inBorders = step(TexCoords.x + off.x, 1.f) * 
			step(0.f, TexCoords.x - off.x) * 
			step(TexCoords.y + off.y, 1.f) * 
			step(0.f, TexCoords.y - off.y);

		vec3 samp = texture(image, TexCoords + off).rgb;
		float weight = bilateralCoeffient(base, samp) * weights[i] * inBorders;
		weightSum += weight;

		//Sample right / top pixel
		result += samp * weight;

		samp = texture(image, TexCoords - off).rgb;
		weight = bilateralCoeffient(base, samp) * weights[i] * inBorders;
		weightSum += weight;

		//Sample left / bottom pixel
		result += samp * weight;
    }

    color = vec4(result / weightSum, 1.f);
}