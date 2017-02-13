#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionDepth;

uniform float focalDistance;
uniform	float aperture;
uniform	float farDistance;
uniform float threshold;
uniform bool horizontal;
uniform float kernel[5];


float getSharpness(float depth) {
	float diff = (focalDistance > depth) ? focalDistance - depth : depth - focalDistance;
	diff = (diff / farDistance) * aperture;
	diff = min(diff, 1.f);

	return (diff < threshold) ? 0.f : diff;
}


void main() {             

	//Size of single texel
    vec2 tex_offset = 1.f / textureSize(image, 0); 
	vec3 baseColor = texture(image, TexCoords).rgb;
    vec3 result = baseColor * kernel[0]; 

    if(horizontal) {
        for(int i = 1; i < 5; i++) {
			float offset = tex_offset.x * i;

			//Sample right pixel
			if(TexCoords.x + offset < 1.f) {
				float depth = texture(gPositionDepth, TexCoords + vec2(offset, 0.f)).w;
				float sharp = getSharpness(depth);

				result += (texture(image, TexCoords + vec2(offset, 0.f)).rgb * sharp + 
					baseColor * (1 - sharp))* kernel[i];
            }

			//Sample left pixel
			if(TexCoords.x - offset > 0.f) {
				depth = texture(gPositionDepth, TexCoords - vec2(offset, 0.f)).w;
				sharp = getSharpness(depth);

				result += (texture(image, TexCoords - vec2(offset, 0.f)).rgb * sharp + 
					baseColor * (1.f - sharp))* kernel[i];
			}
        }
    }
    else {
        for(int i = 1; i < 5; i++) {
			float offset = tex_offset.y * i;
			
			//Sample up pixel
			if(TexCoords.y + offset > 1.f) {
				float depth = texture(gPositionDepth, TexCoords + vec2(0.f, offset)).w;
				float sharp = getSharpness(depth);

				result += (texture(image, TexCoords + vec2(0.f, offset)).rgb * sharp + 
					baseColor * (1.f - sharp))* kernel[i];
		
			}

			//Sample down pixel
			if(TexCoords.y - offset < 0.f) {
				depth = texture(gPositionDepth, TexCoords - vec2(0.f, offset)).w;
				sharp = getSharpness(depth);

				result += (texture(image, TexCoords - vec2(0.f, offset)).rgb * sharp + 
					baseColor * (1.f - sharp))* kernel[i];
			}
        }
    }

    color = vec4(result, 1.f);
}
