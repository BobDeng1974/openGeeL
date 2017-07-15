#include <vec3.hpp>
#include "../shader/rendershader.h"
#include "../primitives/screenquad.h"
#include "../framebuffer/framebuffer.h"
#include "../texturing/imagetexture.h"
#include "drawdefault.h"

using namespace glm;

namespace geeL {

	DefaultPostProcess::DefaultPostProcess(float exposure)
		: PostProcessingEffect("renderer/postprocessing/drawdefault.frag"), exposure(exposure) {
	
		noise = new ImageTexture("resources/textures/noise.png", ColorType::Single);
	}

	DefaultPostProcess::DefaultPostProcess(const DefaultPostProcess & other) 
		: PostProcessingEffect(other), exposure(other.exposure) {

		noise = new ImageTexture("resources/textures/noise.png", ColorType::Single);
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
	}


	void DefaultPostProcess::init(ScreenQuad& screen, IFrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.addMap(*noise, "noiseMap");
		shader.setVector3("noiseScale",
			vec3(float(buffer.getWidth()) / 255.f, float(buffer.getHeight()) / 255.f, 0.f));

		exposureLocation = shader.setFloat("exposure", exposure);
	}


	float DefaultPostProcess::getExposure() const {
		return exposure;
	}

	void DefaultPostProcess::setExposure(float exposure) {
		if (exposure > 0.f && exposure != this->exposure) {
			this->exposure = exposure;

			shader.use();
			shader.setFloat(exposureLocation, exposure);
		}
			
	}

}