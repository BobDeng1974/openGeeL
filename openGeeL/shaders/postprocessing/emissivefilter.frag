#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D emission;

uniform float scatter;


void main() {             
    vec3 result = texture(image, TexCoords).rgb; 
	vec3 emissivity = texture(emission, TexCoords).rgb; 

	float brightness = dot(emissivity, vec3(0.2126, 0.7152, 0.0722));

	color = step(scatter, brightness) * vec4(result, 1.0f);
}
