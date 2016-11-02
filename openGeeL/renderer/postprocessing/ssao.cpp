#define GLEW_STATIC
#include <glew.h>
#include <random>
#include <string>
#include <glm.hpp>
#include "../utility/screenquad.h"
#include "../cameras/camera.h"
#include "ssao.h"

using namespace glm;
using namespace std;


namespace geeL {

	SSAO::SSAO(const Camera& camera, GaussianBlur& blur, float radius)
		: PostProcessingEffect("renderer/postprocessing/ssao.frag")
		, camera(camera), blur(blur), radius(radius) {
	
		uniform_real_distribution<GLfloat> random(0.f, 1.f);
		default_random_engine generator;

		// Sample kernel
		for (unsigned int i = 0; i < sampleCount; ++i) {
			vec3 sample = vec3(random(generator) * 2.f - 1.f, 
				random(generator) * 2.f - 1.f, 
				random(generator));
			
			sample = normalize(sample);
			sample *= random(generator);
			float scale = float(i) / 64.f;

			scale = 0.1f + (scale * scale) * 0.9f;
			sample *= scale;
			kernel.push_back(sample);
		}
		
		//Sample noise
		for (unsigned int i = 0; i < 16; ++i) {
			vec3 sample = vec3(random(generator) * 2.f - 1.f,
				random(generator) * 2.f - 1.f,
				0.f);

			noise.push_back(sample);
		}

		noiseTexture = SimpleTexture(noise, 4, 4, GL_REPEAT, GL_NEAREST);
	}

	void SSAO::setScreen(ScreenQuad& screen) {
		PostProcessingEffect::setScreen(screen);

		buffers.push_back(noiseTexture.GetID());
	}


	void SSAO::bindValues() {
		shader.setInteger("gPositionDepth", shader.mapOffset);
		shader.setInteger("gNormal", shader.mapOffset + 1);
		shader.setInteger("noiseTexture", shader.mapOffset + 2);

		shader.setFloat("screenWidth", screen->width);
		shader.setFloat("screenHeight", screen->height);
		shader.setFloat("radius", radius);

		for (unsigned int i = 0; i < sampleCount; i++)
			shader.setVector3("samples[" + to_string(i) + "]", kernel[i]);

		shader.setMat4("projection", camera.getProjectionMatrix());
	}
}