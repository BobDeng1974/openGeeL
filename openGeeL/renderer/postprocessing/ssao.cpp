#define GLEW_STATIC
#include <glew.h>
#include <random>
#include <string>
#include <glm.hpp>
#include "../primitives/screenquad.h"
#include "../texturing/rendertexture.h"
#include "gaussianblur.h"
#include "ssao.h"

using namespace glm;
using namespace std;


namespace geeL {

	SSAO::SSAO(PostProcessingEffectFS& blur, float radius, ResolutionScale resolution)
		: PostProcessingEffectFS("renderer/postprocessing/ssao.frag"), blur(blur), radius(radius), resolution(resolution) {
	
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

	SSAO::SSAO(const SSAO & other) : PostProcessingEffectFS(other), radius(other.radius), blur(other.blur), 
		noise(other.noise), resolution(other.resolution) {
		
		noiseTexture = new ImageTexture(noise, 4, 4, WrapMode::Repeat, FilterMode::None);
	}

	SSAO::~SSAO() {
		delete noiseTexture;

		if (tempTexture != nullptr) delete tempTexture;
	}

	
	SSAO& SSAO::operator=(const SSAO& other) {
		if (&other != this) {
			SSAO s(other);
			*this = std::move(s);
		}

		return *this;
	}

	void SSAO::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.set<float>("screenWidth", float(resolution.getWidth()));
		shader.set<float>("screenHeight", float(resolution.getHeight()));
		shader.set<float>("radius", radius);

		for (unsigned int i = 0; i < sampleCount; i++)
			shader.set<glm::vec3>("samples[" + to_string(i) + "]", kernel[i]);
		
		tempTexture = new RenderTexture(resolution, ColorType::Single, WrapMode::Repeat, FilterMode::None);

		blur.init(screen, buffer, resolution);
		blur.setImageBuffer(*tempTexture);

		projectionLocation = shader.getLocation("projection");
	}

	void SSAO::draw() {
		parentBuffer->add(*tempTexture);
		parentBuffer->fill([this]() {
			shader.use();
			bindValues();
			bindToScreen();
		});

		blur.draw();
	}

	void SSAO::bindValues() {
		shader.set<glm::mat4>(projectionLocation, camera->getProjectionMatrix());
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
			shader.set<float>("radius", radius);
		}
	}
	
	const ResolutionScale& SSAO::getResolution() const {
		return resolution;
	}
}

