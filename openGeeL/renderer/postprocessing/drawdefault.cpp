#include <vec3.hpp>
#include "shader/rendershader.h"
#include "primitives/screenquad.h"
#include "framebuffer/framebuffer.h"
#include "texturing/imagetexture.h"
#include "texturing/textureprovider.h"
#include "drawdefault.h"

using namespace glm;

namespace geeL {

	DefaultPostProcess::DefaultPostProcess(float exposure)
		: PostProcessingEffectFS("renderer/postprocessing/drawdefault.frag")
		, customTexture(nullptr) {
	
		noise = new ImageTexture("resources/textures/noise.png", ColorType::Single);
		shader.setValue("exposure", exposure, Range<float>(0.f, 100.f));
	}

	DefaultPostProcess::DefaultPostProcess(const DefaultPostProcess& other) 
		: PostProcessingEffectFS(other)
		, customTexture(other.customTexture) {

		noise = new ImageTexture("resources/textures/noise.png", ColorType::Single);
		shader.setValue("exposure", other.getExposure(), Range<float>(0.f, 100.f));
	}

	DefaultPostProcess::~DefaultPostProcess() {
		delete noise;
	}

	DefaultPostProcess & DefaultPostProcess::operator=(const DefaultPostProcess& other) {
		if (&other != this) {
			DefaultPostProcess s(other);
			*this = std::move(s);
		}

		return *this;
	}


	void DefaultPostProcess::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		shader.addMap(*noise, "noiseMap");
		shader.bind<glm::vec3>("noiseScale",
			vec3(float(resolution.getWidth()) / 255.f, float(resolution.getHeight()) / 255.f, 0.f));
	}

	void DefaultPostProcess::draw() {
		if(customTexture == nullptr)
			setImage(provider->requestCurrentImage());

		PostProcessingEffectFS::draw();
	}

	void DefaultPostProcess::setCustomImage(const Texture* const texture) {
		customTexture = texture;

		if (customTexture != nullptr)
			setImage(*customTexture);
	}


	float DefaultPostProcess::getExposure() const {
		return shader.getFloatValue("exposure");
	}

	void DefaultPostProcess::setExposure(float exposure) {
		shader.setValue("exposure", exposure, Range<float>(0.f, 100.f));
	}

}