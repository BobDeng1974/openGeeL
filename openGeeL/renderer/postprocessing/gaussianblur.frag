#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform bool horizontal;
uniform float kernel[5];


void main() {             

	//Size of single texel
    vec2 texOffset = 1.f / textureSize(image, 0); 
    vec3 result = texture(image, TexCoords).rgb * kernel[0]; 

    if(horizontal) {
        for(int i = 1; i < 5; i++) {
			float offset = texOffset.x * i;

			//Sample right pixel
			if(TexCoords.x + offset < 1.f)
				result += texture(image, TexCoords + vec2(offset, 0.f)).rgb * kernel[i];
            
			//Sample left pixel
			if(TexCoords.x - offset > 0.f)
				result += texture(image, TexCoords - vec2(offset, 0.f)).rgb * kernel[i];
        }
    }
    else {
        for(int i = 1; i < 5; i++) {
			float offset = texOffset.y * i;
			
			//Sample up pixel
			if(TexCoords.y + offset < 1.f)
				result += texture(image, TexCoords + vec2(0.f, offset)).rgb * kernel[i];

			//Sample down pixel
			if(TexCoords.y - offset > 0.f)
				result += texture(image, TexCoords - vec2(0.f, offset)).rgb * kernel[i];
        }
    }

    color = vec4(result, 1.f);
}
