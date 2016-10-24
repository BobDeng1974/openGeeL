#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform bool horizontal;
uniform float kernel[5];


void main() {             

	//Size of single texel
    vec2 tex_offset = 1.0f / textureSize(image, 0); 
    vec3 result = texture(image, TexCoords).rgb * kernel[0]; 

    if(horizontal) {
        for(int i = 1; i < 5; i++) {

			//Sample right pixel
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * kernel[i];
            
			//Sample left pixel
			result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * kernel[i];
        }
    }
    else {
        for(int i = 1; i < 5; i++) {
			
			//Sample up pixel
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * kernel[i];

			//Sample down pixel
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * kernel[i];
        }
    }

    color = vec4(result, 1.0);
}
