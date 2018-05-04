
#define FOG 1


#if (FOG == 1)

uniform vec3 fogColor = vec3(0.f);
uniform float fogFalloff = 100.f;
uniform float fogFactor = 2.f;


vec3 computeFog(vec3 position) {
	float fogFactor = clamp(pow(abs(position.z) / fogFalloff, fogFactor), 0.f, 1.f);
	return fogColor * fogFactor;
}

vec3 blendFog(vec3 position, vec3 color) {
	float fogFactor = clamp(pow(abs(position.z) / fogFalloff, fogFactor), 0.f, 1.f);
	return fogColor * fogFactor + color * (1.f - fogFactor);

}

#endif
