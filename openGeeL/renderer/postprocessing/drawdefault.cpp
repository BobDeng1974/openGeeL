#include <vec3.hpp>
#include "../shader/rendershader.h"
#include "../primitives/screenquad.h"
#include "../framebuffer/framebuffer.h"
#include "../texturing/imagetexture.h"
#include "drawdefault.h"

using namespace glm;

namespace geeL {

	DefaultPostProcess::DefaultPostProcess(float exposure)
		: PostProcessingEffectFS("renderer/postprocessing/drawdefault.frag") {
	
		noise = new ImageTexture("resources/textures/noise.png", ColorType::Single);
		shader.setValue("exposure", exposure, POSITIVE_FLOAT_RANGE);
	}

	DefaultPostProcess::DefaultPostProcess(const DefaultPostProcess& other) 
		: PostProcessingEffectFS(other) {

		noise = new ImageTexture("resources/textures/noise.png", ColorType::Single);
		shader.setValue("exposure", other.getExposure(), POSITIVE_FLOAT_RANGE);
	}

	DefaultPostProcess::~DefaultPostProcess() {
		noise->remove();
		delete noise;
	}

	DefaultPostProcess & DefaultPostProcess::operator=(const DefaultPostProcess& other) {
		if (&other != this) {
			DefaultPostProcess s(other);
			*this = std::move(s);
		}

		return *this;
	}


	void DefaultPostProcess::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.addMap(*noise, "noiseMap");
		shader.bind<glm::vec3>("noiseScale",
			vec3(float(resolution.getWidth()) / 255.f, float(resolution.getHeight()) / 255.f, 0.f));
	}


	float DefaultPostProcess::getExposure() const {
		return shader.getFloatValue("exposure");
	}

	void DefaultPostProcess::setExposure(float exposure) {
		shader.setValue("exposure", exposure, POSITIVE_FLOAT_RANGE);
	}

}