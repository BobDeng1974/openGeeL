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
uniform float falloff;

void main() {     
	vec3 baseColor = texture(image, TexCoords).rgb;

	//Size of single texel
    vec2 texOffset = 1.f / textureSize(image, 0); 
    vec3 result = vec3(baseColor.r * weightsR[0], baseColor.g * weightsG[0], baseColor.b * weightsB[0]);
	

	float hor = step(1.f, float(horizontal));
	float ver = 1.f - hor;

	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < kernelSize; i++) {
		vec3 weight = vec3(weightsR[i], weightsG[i], weightsB[i]);

		//Sample right / top pixel
		result += vec3(
			texture(image, TexCoords + offset * offsetsR[i]).r * weight.r,
			texture(image, TexCoords + offset * offsetsG[i]).g * weight.g,
			texture(image, TexCoords + offset * offsetsB[i]).b * weight.b);
            
		//Sample left / bottom pixel
		result += vec3(
			texture(image, TexCoords - offset * offsetsR[i]).r * weight.r,
			texture(image, TexCoords - offset * offsetsG[i]).g * weight.g,
			texture(image, TexCoords - offset * offsetsB[i]).b * weight.b);

    }

    color = vec4(result, 1.f);
}
