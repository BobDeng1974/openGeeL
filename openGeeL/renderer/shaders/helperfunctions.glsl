

float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0f);
}

float random(vec3 seed, int i) {
	vec4 seed4 = vec4(seed, i);
	float dt = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	float sn = mod(dt,3.14);

	return fract(sin(sn) * 43758.5453);
}