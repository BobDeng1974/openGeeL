#version 430 core

in vec4 fragPosition;

void main() {
	float depth = fragPosition.z / fragPosition.w;
	depth = depth * 0.5f + 0.5f;

	float moment2 = depth * depth; 

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	moment2 += 0.25f * (dx * dx + dy * dy);

	gl_FragColor = vec4(depth, moment2, vec2(0.f));
}