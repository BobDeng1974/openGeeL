#version 430 core

in vec2 TexCoords;

out vec4 color;

const int kernelSize = 4;

uniform float weightsR[kernelSize];
uniform float offsetsR[kernelSize];

uniform float weightsG[kernelSize];
uniform float offsetsG[kernelSize];

uniform float weightsB[kernelSize];
uniform float offsetsB[kernelSize];

uniform sampler2D image;
uniform bool horizontal;


void main() {             

	//Size of single texel
    vec2 texOffset = 1.f / textureSize(image, 0); 
    vec3 result = vec3(
			texture(image, TexCoords).r * weightsR[0],
			texture(image, TexCoords).g * weightsG[0],
			texture(image, TexCoords).b * weightsB[0]);

	float hor = step(1.f, float(horizontal));
	float ver = 1.f - hor;

	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < kernelSize; i++) {

		//Sample right / top pixel
		result += vec3(
			texture(image, TexCoords + offset * offsetsR[i]).r * weightsR[i],
			texture(image, TexCoords + offset * offsetsG[i]).g * weightsG[i],
			texture(image, TexCoords + offset * offsetsB[i]).b * weightsB[i]);
            
		//Sample left / bottom pixel
		result += vec3(
			texture(image, TexCoords - offset * offsetsR[i]).r * weightsR[i],
			texture(image, TexCoords - offset * offsetsG[i]).g * weightsG[i],
			texture(image, TexCoords - offset * offsetsB[i]).b * weightsB[i]);

    }

    color = vec4(result, 1.f);
}
