#include <vec3.hpp>
#include "../shader/shader.h"
#include "../primitives/screenquad.h"
#include "../framebuffer/framebuffer.h"
#include "../texturing/simpletexture.h"
#include "drawdefault.h"

using namespace glm;

namespace geeL {

	DefaultPostProcess::DefaultPostProcess(float exposure)
		: PostProcessingEffect("renderer/postprocessing/drawdefault.frag"), exposure(exposure) {}


	void DefaultPostProcess::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		SimpleTexture noise = SimpleTexture("resources/textures/noise.png", ColorType::Single);
		buffers.push_back(noise.getID());

		shader.setVector3("noiseScale",
			vec3(float(info.width) / 255.f, float(info.height) / 255.f, 0.f));

		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("noiseMap", shader.mapOffset + 1);
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