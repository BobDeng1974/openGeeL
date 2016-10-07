#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform float gamma;
uniform sampler2D screenTexture;


void main() {
	color = texture(screenTexture, TexCoords).rgba;
	
	color.rgb = pow(color.rgb, vec3(1.0f / gamma));

	//gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f));
	//gl_FragColor.a = 1;
}
