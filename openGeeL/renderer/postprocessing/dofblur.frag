#version 330 core

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


bool isSharp(float depth) {
	float diff = (focalDistance > depth) ? focalDistance - depth : depth - focalDistance;
	diff = (diff / farDistance) * aperture;

	return min(diff, 1.f) < threshold;
}


void main() {             

	//Size of single texel
    vec2 tex_offset = 1.0f / textureSize(image, 0); 
	vec3 baseColor = texture(image, TexCoords).rgb;
    vec3 result = baseColor * kernel[0]; 
	float depth = texture(gPositionDepth, TexCoords).w;

    if(horizontal) {
        for(int i = 1; i < 5; i++) {
			float offset = tex_offset.x * i;

			if(offset >= 0.f) {
				//Sample right pixel
				float rightDepth = texture(gPositionDepth, TexCoords + vec2(offset, 0.0)).w;

				if(!isSharp(rightDepth))
					result += texture(image, TexCoords + vec2(offset, 0.0)).rgb * kernel[i];
				else
					result += baseColor * kernel[i];
            
				//Sample left pixel
				float leftDepth = texture(gPositionDepth, TexCoords - vec2(offset, 0.0)).w;
				if(!isSharp(leftDepth))
					result += texture(image, TexCoords - vec2(offset, 0.0)).rgb * kernel[i];
				else
					result += baseColor * kernel[i];
			}
        }
    }
    else {
        for(int i = 1; i < 5; i++) {
			float offset = tex_offset.y * i;
			
			if(offset <= 1.f) {
				//Sample up pixel
				float upDepth = texture(gPositionDepth, TexCoords + vec2(0.0, offset)).w;

				if(!isSharp(upDepth))
					result += texture(image, TexCoords + vec2(0.0, offset)).rgb * kernel[i];
				else
					result += baseColor * kernel[i];

				//Sample down pixel
				float downDepth = texture(gPositionDepth, TexCoords - vec2(0.0, offset)).w;

				if(!isSharp(downDepth))
					result += texture(image, TexCoords - vec2(0.0, offset)).rgb * kernel[i];
				else
					result += baseColor * kernel[i];
			}
        }
    }

    color = vec4(result, 1.0);
}
