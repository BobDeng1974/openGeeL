#version 430 core

in vec2 TexCoords;

out vec4 color;

const int kernelSize = 3;

uniform float weights[kernelSize];
uniform float offsets[kernelSize];

uniform sampler2D image;
uniform sampler2D sobel;

uniform bool horizontal;


void main() {

	//Size of single texel
    vec2 texOffset = 1.f / textureSize(image, 0); 
	vec3 base = texture(image, TexCoords).rgb;
    vec3 result = base * weights[0]; 
	float blurStrength = min(1.f, texture(sobel, TexCoords).r);

	float hor = step(1.f, float(horizontal));
	float ver = 1.f - hor;

	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < kernelSize; i++) {
		vec2 off = offset * offsets[i];

		//Check if image borders aren't crossed
		float inBorders = step(TexCoords.x + off.x, 1.f) * 
			step(0.f, TexCoords.x - off.x) * 
			step(TexCoords.y + off.y, 1.f) * 
			step(0.f, TexCoords.y - off.y);

		//Sample right / top pixel
		result += inBorders * 
			texture(image, TexCoords + off).rgb * weights[i];
            
		//Sample left / bottom pixel
		result += inBorders *
			texture(image, TexCoords - off).rgb * weights[i];
    }

    color = vec4(result * blurStrength + base * (1.f - blurStrength) , 1.f);
	//color = vec4(vec3(texture(sobel, TexCoords).r), 1.f);
}

