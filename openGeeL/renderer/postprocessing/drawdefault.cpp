#include <vec3.hpp>
#include "shader/rendershader.h"
#include "shader/shaderreader.h"
#include "primitives/screenquad.h"
#include "framebuffer/framebuffer.h"
#include "texturing/imagetexture.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "drawdefault.h"

using namespace glm;

namespace geeL {

	DefaultPostProcess::DefaultPostProcess(float exposure, TonemappingMethod method)
		: PostProcessingEffectFS(defaultVertexPath, "shaders/postprocessing/drawdefault.frag", 
			StringReplacement("^#define TONEMAPPING_METHOD\\s+([0-9]+){1}\\s?",
				std::to_string((int)method), 1))
		, customTexture(nullptr) {
	
		noise = ImageTexture::create<ImageTexture>("resources/textures/noise.png", ColorType::Single);
		shader.setValue("exposure", exposure, Range<float>(0.f, 100.f));
	}

	DefaultPostProcess::DefaultPostProcess(const DefaultPostProcess& other) 
		: PostProcessingEffectFS(other)
		, customTexture(other.customTexture) {

		noise = ImageTexture::create<ImageTexture>("resources/textures/noise.png", ColorType::Single);
		shader.setValue("exposure", other.getExposure(), Range<float>(0.f, 100.f));
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

	void DefaultPostProcess::setCustomImage(const ITexture* const texture) {
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