#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;

uniform float bias;
uniform float scale;

void main() {         
    vec3 result = (texture(image, TexCoords).rgb + bias) * scale; 
	
	color = vec4(max(vec3(0.f), result), 1.f);
}
