#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform bool horizontal;
uniform float kernel[5];


void main() {             

	//Size of single texel
    vec2 texOffset = 1.f / textureSize(image, 0); 
    vec3 base = texture(image, TexCoords).rgb * kernel[0]; 
	vec3 result = base;

	float hor = step(1.f, float(horizontal));
	float ver = 1 - hor;

	float d = 0.001f;
	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < 5; i++) {
		vec2 off = offset * i;

		//Check if image borders aren't crossed
		float inBorders = step(TexCoords.x + off.x, 1.f) * 
			step(0.f, TexCoords.x - off.x) * 
			step(TexCoords.y + off.y, 1.f) * 
			step(0.f, TexCoords.y - off.y);

		vec3 samp = texture(image, TexCoords + off).rgb;
		float dist = 1.f - min(1.f, distance(base, samp) * 0.5f);
		d += dist * kernel[i];

		//Sample right / top pixel
		result +=  inBorders * samp * kernel[i] * dist;
            

		samp = texture(image, TexCoords - off).rgb;
		dist = length(base - samp);
		d += dist * kernel[i];

		//Sample left / bottom pixel
		result += inBorders * samp * kernel[i] * dist;
    }



    color = vec4(result / d, 1.f);
}