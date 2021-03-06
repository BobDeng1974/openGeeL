#define GLEW_STATIC
#include <glew.h>
#include <random>
#include <string>
#include <glm.hpp>
#include "primitives/screenquad.h"
#include "framebuffer/framebuffer.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "glwrapper/glguards.h"
#include "gaussianblur.h"
#include "ssao.h"

using namespace glm;
using namespace std;


namespace geeL {

	SSAO::SSAO(PostProcessingEffectFS& blur, float radius, const ResolutionPreset& resolution)
		: PostProcessingEffectFS("shaders/postprocessing/ssao.frag")
		, blendEffect(new PostProcessingEffectFS("shaders/postprocessing/ssaoblend.frag"))
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
		std::vector<glm::vec3> noise;
		for (unsigned int i = 0; i < 16; ++i) {
			vec3 sample = vec3(random(generator) * 2.f - 1.f,
				random(generator) * 2.f - 1.f, 0.f);

			noise.push_back(sample);
		}

		noiseTexture = new Texture2D(Resolution(4, 4), ColorType::RGB16, FilterMode::None, 
			WrapMode::Repeat, AnisotropicFilter::None, &noise[0]);
	}

	SSAO::~SSAO() {
		delete noiseTexture;

		if (ssaoTexture != nullptr) delete ssaoTexture;
		if (blurTexture != nullptr) delete blurTexture;
		if (blendEffect != nullptr) delete blendEffect;
	}

	

	void SSAO::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		assert(provider != nullptr);
		addTextureSampler(provider->requestPosition(), "gPosition");
		addTextureSampler(provider->requestNormal(), "gNormal");
		addTextureSampler(*noiseTexture, "noiseTexture");

		const Resolution& resolution = Resolution(parameter.resolution, scale);

		shader.bind<float>("screenWidth", float(resolution.getWidth()));
		shader.bind<float>("screenHeight", float(resolution.getHeight()));
		shader.bind<float>("radius", radius);

		for (unsigned int i = 0; i < sampleCount; i++)
			shader.bind<glm::vec3>("samples[" + to_string(i) + "]", kernel[i]);

		ssaoTexture = &provider->requestTextureManual(scale, ColorType::Single, FilterMode::None, WrapMode::Repeat);
		blur.init(PostProcessingParameter(parameter, resolution));
		blur.setImage(*ssaoTexture);

		projectionLocation = shader.getLocation("projection");

		blurTexture = &provider->requestTextureManual(scale, ColorType::Single, FilterMode::None, WrapMode::Repeat);
		blendEffect->init(PostProcessingParameter(ScreenQuad::get(), parameter.buffer, parameter.resolution));
		blendEffect->setImage(*blurTexture);

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
		parentBuffer->add(*blurTexture);
		parentBuffer->fill([this]() {
			blur.draw();
		}, clearColor);

		BlendGuard blendGuard;
		glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE_MINUS_DST_COLOR, GL_DST_COLOR);

		blendEffect->draw();
	}

	void SSAO::fill() {
		if (parentBuffer != nullptr) {
			parentBuffer->add(provider->requestProperties());
			parentBuffer->fill(*this, clearNothing);
		}
	}

	void SSAO::bindValues() {
		camera->bindProjectionMatrix(shader, projectionLocation);

		blur.bindValues();
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

