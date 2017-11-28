
const float PI = 3.14159265359;

float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0f);
}

vec3 orthogonal(vec3 v) {
	v = normalize(v);
	vec3 o = vec3(1.f, 0.f, 0.f);

	return abs(dot(v, o)) > 0.999f ? cross(v, vec3(0.f, 1.f, 0.f)) : cross(v, o);
}

