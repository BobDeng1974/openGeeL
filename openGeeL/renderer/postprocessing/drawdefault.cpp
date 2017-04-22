#include <vec3.hpp>
#include "../shader/shader.h"
#include "../primitives/screenquad.h"
#include "../framebuffer/framebuffer.h"
#include "../texturing/imagetexture.h"
#include "drawdefault.h"

using namespace glm;

namespace geeL {

	DefaultPostProcess::DefaultPostProcess(float exposure)
		: PostProcessingEffect("renderer/postprocessing/drawdefault.frag"), exposure(exposure) {}


	void DefaultPostProcess::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		ImageTexture noise = ImageTexture("resources/textures/noise.png", ColorType::Single);
		shader.addMap(noise.getID(), "noiseMap");

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