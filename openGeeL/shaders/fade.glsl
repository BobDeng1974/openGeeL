
#define FADE 1

#if (FADE == 1)
	
uniform float fadeFalloff = 200.f;
uniform float fadeFactor = 1.f;

float computeFade(vec3 position) {
	return 1.f - clamp(pow(abs(position.z) / fadeFalloff, fadeFactor), 0.f, 1.f);
}

#endif