#define GLEW_STATIC
#include <glew.h>
#include <random>
#include <string>
#include <glm.hpp>
#include "../utility/screenquad.h"
#include "../cameras/camera.h"
#include "gaussianblur.h"
#include "ssao.h"

using namespace glm;
using namespace std;


namespace geeL {

	SSAO::SSAO(const Camera& camera, BilateralFilter& blur, float radius)
		: WorldPostProcessingEffect("renderer/postprocessing/ssao.frag"),
			camera(camera), blur(blur), radius(radius) {
	
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
				random(generator) * 2.f - 1.f, 0.f);

			noise.push_back(sample);
		}

		noiseTexture = SimpleTexture(noise, 4, 4, GL_REPEAT, GL_NEAREST);
	}

	void SSAO::init(ScreenQuad& screen) {
		PostProcessingEffect::init(screen);

		tempBuffer.init(screen.width, screen.height, 1, Single, GL_NEAREST, false);

		blur.init(screen);
		blur.setParentFBO(parentFBO);
	}

	void SSAO::draw() {
		if (ssaoPass) {
			ssaoPass = !ssaoPass;
			bindToScreen();
		}
		else {
			ssaoPass = !ssaoPass;

			shader.use();
			bindValues();

			FrameBuffer::bind(parentFBO);
			blur.draw();
		}
	}

	void SSAO::bindValues() {
		shader.setInteger("gPositionDepth", shader.mapOffset);
		shader.setInteger("gNormalMet", shader.mapOffset + 1);
		shader.setInteger("noiseTexture", shader.mapOffset + 2);

		shader.setFloat("screenWidth", screen->width);
		shader.setFloat("screenHeight", screen->height);
		shader.setFloat("radius", radius);

		for (unsigned int i = 0; i < sampleCount; i++)
			shader.setVector3("samples[" + to_string(i) + "]", kernel[i]);

		shader.setMat4("projection", camera.getProjectionMatrix());

		tempBuffer.fill(*this, false);
	}

	WorldMaps SSAO::requiredWorldMaps() const {
		return (WorldMaps::PositionDepth | WorldMaps::NormalMetallic);
	}

	WorldMatrices SSAO::requiredWorldMatrices() const {
		return WorldMatrices::None;
	}

	WorldVectors SSAO::requiredWorldVectors() const {
		return WorldVectors::None;
	}

	list<WorldMaps> SSAO::requiredWorldMapsList() const {
		return { WorldMaps::PositionDepth, WorldMaps::NormalMetallic };
	}

	void SSAO::addWorldInformation(list<unsigned int> maps,
		list<const mat4*> matrices, list<const vec3*> vectors) {

		if (maps.size() != 2)
			throw "Wrong number of texture maps attached to SSAO";

		blur.setBuffer({ tempBuffer.getColorID(), maps.front() });
		setBuffer(maps);
		buffers.push_back(noiseTexture.GetID());
	}
}