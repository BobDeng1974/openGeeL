#version 430 core

in vec2 TexCoords;

out vec4 color;

const int kernelSize = 9;

uniform sampler2D image;
uniform sampler2D gPositionDepth;

uniform float focalDistance;
uniform	float aperture;
uniform	float farDistance;
uniform float threshold;
uniform bool horizontal;
uniform float kernel[kernelSize];


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
    vec3 result = baseColor * kernel[0]; 

	float hor = step(1.f, float(horizontal));
	float ver = 1.f - hor;

	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < kernelSize; i++) {
		vec2 off = offset * i;

		//Check if image borders aren't crossed
		float inBorders = step(TexCoords.x + off.x, 1.f) * 
			step(0.f, TexCoords.x - off.x) * 
			step(TexCoords.y + off.y, 1.f) * 
			step(0.f, TexCoords.y - off.y);

		//Sample right / top pixel
		float depth = -texture(gPositionDepth, TexCoords + off).z;
		float sharp = getSharpness(depth);

		result += inBorders * 
			(texture(image, TexCoords + off).rgb * sharp + baseColor * (1 - sharp)) * kernel[i];
            
		//Sample left / bottom pixel
		depth = -texture(gPositionDepth, TexCoords - off).z;
		sharp = getSharpness(depth);

		result += inBorders * 
			(texture(image, TexCoords - off).rgb * sharp + baseColor * (1.f - sharp))* kernel[i];
    }

    color = vec4(clamp(result, 0.f, 1.f), 1.f);
}
