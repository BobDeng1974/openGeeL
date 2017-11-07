#version 430 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform sampler2D image2;


void main() { 
	vec4 a = texture(image, TexCoords);
	vec4 b = texture(image2, TexCoords);

	float alpha = clamp(a.a, 0.f, 1.f);
	vec3 base = (1.f - alpha) * a.rgb + alpha * b.rgb;

	color = vec4(base, 1.f);
}