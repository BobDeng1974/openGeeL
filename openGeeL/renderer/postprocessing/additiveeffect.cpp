#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "framebuffer/framebuffer.h"
#include "shader/shader.h"
#include "utility/glguards.h"
#include "utility/clearing.h"
#include "additiveeffect.h"

namespace geeL {

	AdditiveEffect::AdditiveEffect(const std::string& fragmentPath, BlendMode mode)
		: PostProcessingEffectFS(fragmentPath), mode(mode) {}

	AdditiveEffect::AdditiveEffect(const std::string& vertexPath, const std::string& fragmentPath, BlendMode mode)
		: PostProcessingEffectFS(vertexPath, fragmentPath), mode(mode) {}


	void AdditiveEffect::setImage(const Texture& texture) {}


	void AdditiveEffect::fill() {
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
			parentBuffer->fill([this]() {
				if(active) PostProcessingEffectFS::draw();
			}, clearNothing);
		}
			
	}


	AdditiveWrapper::AdditiveWrapper(PostProcessingEffectFS& effect, BlendMode mode)
		: AdditiveEffect("renderer/shaders/screen.frag", mode), effect(effect), tempTexture(nullptr) {}

	AdditiveWrapper::~AdditiveWrapper() {
		if (tempTexture != nullptr) delete tempTexture;
	}


	const Texture& AdditiveWrapper::getImage() const {
		return effect.getImage();
	}

	void AdditiveWrapper::setImage(const Texture& texture) {
		effect.setImage(texture);
	}

	void AdditiveWrapper::addTextureSampler(const Texture& texture, const std::string& name) {
		effect.addTextureSampler(texture, name);
	}

	void AdditiveWrapper::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		effect.init(PostProcessingParameter(parameter, parameter.resolution));

		tempTexture = new RenderTexture(parameter.resolution, ColorType::RGB16,
			WrapMode::ClampEdge, FilterMode::Linear);

		PostProcessingEffectFS::setImage(*tempTexture);
	}

	void AdditiveWrapper::fill() {

		if (active && parentBuffer != nullptr) {
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