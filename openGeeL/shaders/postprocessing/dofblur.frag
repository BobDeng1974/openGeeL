#version 430 core

#define POSITION_MAP gPositionDepth

in vec2 TexCoords;

out vec4 color;

const int kernelSize = 5;

uniform float weights[kernelSize];
uniform float offsets[kernelSize];

uniform sampler2D image;
uniform sampler2D POSITION_MAP;

uniform float focalDistance;
uniform	float aperture;
uniform	float farDistance;
uniform float threshold;
uniform bool horizontal;


float getSharpness(float depth) {
	float diff = (focalDistance > depth) ? focalDistance - depth : depth - focalDistance;
	diff = (diff / farDistance) * aperture;
	diff = min(diff, 1.f);

	return (diff < threshold) ? 0.f : diff;
}


void main() {             

	//Size of single texel
    vec2 texOffset = 1.f / textureSize(image, 0); 
	vec3 baseColor = texture(image, TexCoords).rgb;
    vec3 result = baseColor * weights[0]; 

	float hor = step(1.f, float(horizontal));
	float ver = 1.f - hor;

	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < kernelSize; i++) {
		vec2 off = offset * offsets[i];

		//Sample right / top pixel
		float depth = -texture(POSITION_MAP, TexCoords + off).z;
		float sharp = getSharpness(depth);

		result += (texture(image, TexCoords + off).rgb * sharp + baseColor * (1 - sharp)) * weights[i];
            
		//Sample left / bottom pixel
		depth = -texture(POSITION_MAP, TexCoords - off).z;
		sharp = getSharpness(depth);

		result += (texture(image, TexCoords - off).rgb * sharp + baseColor * (1.f - sharp))* weights[i];
    }

    color = vec4(clamp(result, 0.f, 1.f), 1.f);
}
