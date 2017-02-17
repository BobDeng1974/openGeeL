#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform float scatter;


void main() {             
    vec3 result = texture(image, TexCoords).rgb; 
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));

	color = step(scatter, brightness) * vec4(result, 1.0f);
}
