#define GLEW_STATIC
#include <glew.h>
#include <random>
#include <string>
#include <glm.hpp>
#include "../primitives/screenquad.h"
#include "gaussianblur.h"
#include "ssao.h"

using namespace glm;
using namespace std;


namespace geeL {

	SSAO::SSAO(PostProcessingEffect& blur, float radius)
		: PostProcessingEffect("renderer/postprocessing/ssao.frag"), blur(blur), radius(radius) {
	
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

		noiseTexture = SimpleTexture(noise, 4, 4, WrapMode::Repeat, FilterMode::Nearest);
	}

	void SSAO::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);
		screenInfo = &info;

		shader.setInteger("gPositionDepth", shader.mapOffset);
		shader.setInteger("gNormalMet", shader.mapOffset + 1);
		shader.setInteger("noiseTexture", shader.mapOffset + 2);

		shader.setFloat("screenWidth", screenInfo->width);
		shader.setFloat("screenHeight", screenInfo->height);
		shader.setFloat("radius", radius);

		for (unsigned int i = 0; i < sampleCount; i++)
			shader.setVector3("samples[" + to_string(i) + "]", kernel[i]);
		
		tempBuffer.init(info.width, info.height, 1, ColorType::Single, 
			FilterMode::Nearest, WrapMode::Repeat, false);

		blur.init(screen, info);
		blur.setBuffer(tempBuffer.getColorID());
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
		shader.setMat4("projection", *projectionMatrix);

		tempBuffer.fill(*this, false);
	}

	void SSAO::addWorldInformation(map<WorldMaps, unsigned int> maps,
		map<WorldMatrices, const glm::mat4*> matrices,
		map<WorldVectors, const glm::vec3*> vectors) {

		//Set instead of add buffers to override the default image buffer since it isn't need for SSAO
		setBuffer( {maps[WorldMaps::PositionDepth], maps[WorldMaps::NormalMetallic]} );
		buffers.push_back(noiseTexture.getID());

		projectionMatrix = matrices[WorldMatrices::Projection];
	}

	float SSAO::getRadius() const {
		return radius;
	}

	void SSAO::setRadius(float radius) {
		if (radius > 0.f && radius != this->radius) {
			this->radius = radius;

			shader.use();
			shader.setFloat("radius", radius);
		}
	}
}

