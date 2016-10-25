#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;


void main() { 

	vec3 hdr = texture(image, TexCoords).rgb;

	//Tone mapping
	float exposure = 1.0f;
	vec3 result = vec3(1.0f) - exp(-hdr * exposure);

	//Gamma correction
	result.rgb = pow(result.rgb, vec3(0.4545f));

	color = vec4(result, 1.0f);
}