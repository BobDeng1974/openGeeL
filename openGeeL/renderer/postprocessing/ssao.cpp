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

	SSAO::SSAO(PostProcessingEffect& blur, float radius, Resolution resolution)
		: PostProcessingEffect("renderer/postprocessing/ssao.frag"), blur(blur), radius(radius), resolution(resolution) {
	
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

		noiseTexture = new ImageTexture(noise, 4, 4, WrapMode::Repeat, FilterMode::None);
	}

	SSAO::SSAO(const SSAO & other) : PostProcessingEffect(other), radius(other.radius), blur(other.blur), 
		noise(other.noise), resolution(other.resolution) {
		
		noiseTexture = new ImageTexture(noise, 4, 4, WrapMode::Repeat, FilterMode::None);
	}

	SSAO::~SSAO() {
		delete noiseTexture;
	}

	
	SSAO & SSAO::operator=(const SSAO& other) {
		if (&other != this) {
			SSAO s(other);
			*this = std::move(s);
		}
	}

	void SSAO::init(ScreenQuad& screen, const ColorBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setFloat("screenWidth", float(buffer.getWidth()));
		shader.setFloat("screenHeight", float(buffer.getHeight()));
		shader.setFloat("radius", radius);

		for (unsigned int i = 0; i < sampleCount; i++)
			shader.setVector3("samples[" + to_string(i) + "]", kernel[i]);
		
		tempBuffer.init(buffer.getWidth(), buffer.getHeight(), ColorType::Single,
			FilterMode::None, WrapMode::Repeat, false);

		blur.init(screen, buffer);
		blur.setImageBuffer(tempBuffer);

		projectionLocation = shader.getLocation("projection");
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

			parentBuffer->bind();
			blur.draw();
		}
	}

	void SSAO::bindValues() {
		shader.setMat4(projectionLocation, camera->getProjectionMatrix());

		tempBuffer.fill(*this);
	}

	void SSAO::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addImageBuffer(*maps[WorldMaps::PositionRoughness], "gPositionDepth");
		addImageBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");
		addImageBuffer(*noiseTexture, "noiseTexture");
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
	
	const Resolution& SSAO::getResolution() const {
		return resolution;
	}
}

