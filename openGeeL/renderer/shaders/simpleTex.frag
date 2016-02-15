#version 330 core

in vec2 ourTex;

out vec4 color;
 
uniform sampler2D tex;

void main() {

	vec2 flippedCoord = vec2(ourTex.x, 1- ourTex.y);

	color = texture(tex, flippedCoord);
}
