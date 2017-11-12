#define GLEW_STATIC
#include <glew.h>
#include <random>
#include <string>
#include <glm.hpp>
#include "primitives/screenquad.h"
#include "texturing/rendertexture.h"
#include "utility/glguards.h"
#include "gaussianblur.h"
#include "ssao.h"

using namespace glm;
using namespace std;


namespace geeL {

	SSAO::SSAO(PostProcessingEffectFS& blur, float radius, const ResolutionPreset& resolution)
		: PostProcessingEffectFS("renderer/postprocessing/ssao.frag")
		, blur(blur)
		, radius(radius)
		, scale(resolution) {
	
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

	SSAO::SSAO(const SSAO& other) : PostProcessingEffectFS(other), radius(other.radius), blur(other.blur), 
		noise(other.noise), scale(other.scale) {
		
		noiseTexture = new ImageTexture(noise, 4, 4, WrapMode::Repeat, FilterMode::None);
	}

	SSAO::~SSAO() {
		delete noiseTexture;

		if (ssaoTexture != nullptr) delete ssaoTexture;
		if (blurTexture != nullptr) delete blurTexture;
		if (blendEffect != nullptr) delete blendEffect;
	}

	
	SSAO& SSAO::operator=(const SSAO& other) {
		if (&other != this) {
			SSAO s(other);
			*this = std::move(s);
		}

		return *this;
	}

	void SSAO::setTargetTexture(const Texture& texture) {
		blend = texture.isAssigned();

		if (blend) {
			//Instanciate blending structures if needed
			if(blurTexture == nullptr)
				blurTexture = new RenderTexture(resolution, ColorType::Single, WrapMode::Repeat, FilterMode::None);
			if (blendEffect == nullptr) {
				blendEffect = new PostProcessingEffectFS("renderer/shaders/screen.frag");
				blendEffect->init(PostProcessingParameter(ScreenQuad::get(), *parentBuffer, resolution));
			}
			
			blendEffect->setImage(*blurTexture);
		}
		else {
			if (blurTexture != nullptr) delete blurTexture;
			if (blendEffect != nullptr) delete blendEffect;
		}

	}

	void SSAO::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		const Resolution& resolution = parameter.resolution;

		shader.bind<float>("screenWidth", float(resolution.getWidth()));
		shader.bind<float>("screenHeight", float(resolution.getHeight()));
		shader.bind<float>("radius", radius);

		for (unsigned int i = 0; i < sampleCount; i++)
			shader.bind<glm::vec3>("samples[" + to_string(i) + "]", kernel[i]);
		
		ssaoTexture = new RenderTexture(resolution, ColorType::Single, WrapMode::Repeat, FilterMode::None);

		blur.init(PostProcessingParameter(parameter, resolution));
		blur.setImage(*ssaoTexture);

		projectionLocation = shader.getLocation("projection");
	}

	void SSAO::draw() {
		if (!active) return;

		parentBuffer->add(*ssaoTexture);
		parentBuffer->fill([this]() {
			bindValues();
			bindToScreen();
		}, clearColor);

		//Draw blurred SSAO in separate texture and
		//then blend it with original texture
		if (blend) {
			parentBuffer->add(*blurTexture);
			parentBuffer->fill([this]() {
				blur.draw();
			}, clearColor);

			BlendGuard blendGuard;
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_DST_COLOR);

			blendEffect->draw();
		}
		//Otherwise, write blurred SSAO directly to target
		else
			blur.draw();
	}

	void SSAO::fill() {
		if (parentBuffer != nullptr)
			parentBuffer->fill(*this, blend ? clearNothing : clearColor);
	}

	void SSAO::bindValues() {
		camera->bindProjectionMatrix(shader, projectionLocation);

		blur.bindValues();
	}

	void SSAO::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addTextureSampler(*maps[WorldMaps::PositionRoughness], "gPositionDepth");
		addTextureSampler(*maps[WorldMaps::NormalMetallic], "gNormalMet");
		addTextureSampler(*noiseTexture, "noiseTexture");
	}

	float SSAO::getRadius() const {
		return radius;
	}

	void SSAO::setRadius(float radius) {
		if (radius > 0.f && radius != this->radius) {
			this->radius = radius;

			shader.bind<float>("radius", radius);
		}
	}
	
	const ResolutionPreset& SSAO::getResolution() const {
		return scale;
	}
}

