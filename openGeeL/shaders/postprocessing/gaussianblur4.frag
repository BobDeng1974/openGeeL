#version 430 core

in vec2 TexCoords;

out vec4 color;

const int kernelSize = 14;

uniform float weights[kernelSize];
uniform float offsets[kernelSize];

uniform sampler2D image;
uniform bool horizontal;


void main() {             

	//Size of single texel
    vec2 texOffset = 1.f / textureSize(image, 0); 
    vec3 result = texture(image, TexCoords).rgb * weights[0]; 

	float hor = step(1.f, float(horizontal));
	float ver = 1.f - hor;

	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < kernelSize; i++) {
		vec2 off = offset * offsets[i];

		//Sample right / top pixel
		result += texture(image, TexCoords + off).rgb * weights[i];
            
		//Sample left / bottom pixel
		result += texture(image, TexCoords - off).rgb * weights[i];
    }

    color = vec4(result, 1.f);
}
