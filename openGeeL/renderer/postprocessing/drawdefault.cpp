#include <vec3.hpp>
#include "../shader/shader.h"
#include "../utility/screenquad.h"
#include "../texturing/simpletexture.h"
#include "drawdefault.h"

using namespace glm;

namespace geeL {

	DefaultPostProcess::DefaultPostProcess(float exposure)
		: PostProcessingEffect("renderer/postprocessing/drawdefault.frag"), exposure(exposure) {}


	void DefaultPostProcess::setScreen(ScreenQuad& screen) {
		PostProcessingEffect::setScreen(screen);

		SimpleTexture noise = SimpleTexture("resources/textures/noise.png", true, Diffuse, ColorSingle);
		buffers.push_back(noise.GetID());

		shader.use();
		shader.setVector3("noiseScale",
			vec3(float(screen.width) / 255.f, float(screen.height) / 255.f, 0.f));
	}


	void DefaultPostProcess::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("noiseMap", shader.mapOffset + 1);
		
		shader.setFloat("exposure", exposure);
	}

}