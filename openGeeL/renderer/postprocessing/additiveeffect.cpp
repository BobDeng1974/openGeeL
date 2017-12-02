#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "framebuffer/framebuffer.h"
#include "shader/shader.h"
#include "texturing/textureprovider.h"
#include "texturing/texturetarget.h"
#include "glwrapper/glguards.h"
#include "glwrapper/clearing.h"
#include "additiveeffect.h"

namespace geeL {

	void AdditiveEffect::draw() {
		if (active) {
			drawSubImages();
			bindToScreen();
		}
	}

	void AdditiveEffect::fill() {
		if (!active) return;

		BlendGuard blend;

		switch (mode) {
			case BlendMode::Add:
				blend.blendAdd();
				break;
			case BlendMode::Override:
				blend.blendAlpha();
				break;
		}

		if (parentBuffer != nullptr) {
			parentBuffer->add(provider->requestCurrentImage());
			parentBuffer->fill([this]() {
				draw();
			}, clearNothing);
		}
			
	}


	AdditiveWrapper::AdditiveWrapper(PostProcessingEffectFS& effect, BlendMode mode)
		: AdditiveEffect(mode, "shaders/screen.frag")
		, effect(effect)
		, tempTexture(nullptr) {}


	AdditiveWrapper::~AdditiveWrapper() {
		if (tempTexture != nullptr) delete tempTexture;
	}


	const ITexture& AdditiveWrapper::getImage() const {
		return effect.getImage();
	}

	void AdditiveWrapper::setImage(const ITexture& texture) {
		effect.setImage(texture);
	}

	void AdditiveWrapper::addTextureSampler(const ITexture& texture, const std::string& name) {
		effect.addTextureSampler(texture, name);
	}

	void AdditiveWrapper::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		effect.init(PostProcessingParameter(parameter, parameter.resolution));

		if (tempTexture == nullptr)
			tempTexture = TextureTarget::createTextureTargetPtr<Texture2D>(parameter.resolution, ColorType::RGB16, 
				FilterMode::Linear, WrapMode::ClampEdge).release();
		//else
		//	tempTexture->resize(parameter.resolution);

		PostProcessingEffectFS::setImage(*tempTexture);
	}

	void AdditiveWrapper::bindValues() {
		effect.bindValues();
	}

	void AdditiveWrapper::fill() {

		if (active && parentBuffer != nullptr) {
			effect.setImage(provider->requestCurrentImage());

			parentBuffer->push(*tempTexture);
			parentBuffer->fill([this]() {
				effect.draw();
			}, clearColor);
		}

		AdditiveEffect::fill();
	}

	std::string AdditiveWrapper::toString() const {
		return "Additive " + effect.toString();
	}

	void AdditiveWrapper::setRenderMask(RenderMask mask) {
		PostProcessingEffectFS::setRenderMask(mask);

		effect.setRenderMask(mask);
	}

	void AdditiveWrapper::setResolution(const Resolution& value) {
		PostProcessingEffectFS::setResolution(value);
		effect.setResolution(value);
	}

	Shader& AdditiveWrapper::getShader() {
		return effect.getShader();
	}

}