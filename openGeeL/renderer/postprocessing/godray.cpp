#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include "../transformation/transform.h"
#include "../lighting/directionallight.h"
#include "../utility/screenquad.h"
#include "../scene.h"
#include "gaussianblur.h"
#include "godray.h"

namespace geeL {

	GodRay::GodRay(const RenderScene& scene, glm::vec3 lightPosition, unsigned int samples) 
		: PostProcessingEffect("renderer/postprocessing/godray.frag"), 
			scene(scene), lightPosition(lightPosition), samples(samples), raysOnly(false) {}


	void GodRay::renderRaysOnly(bool only) {
		raysOnly = only;
	}

	void GodRay::bindValues() {
		PostProcessingEffect::bindValues();

		shader.setInteger("samples", samples);
		glm::vec3 screenPos = scene.TranslateToScreenSpace(lightPosition);
		shader.setVector3("lightPosition", screenPos);

		glm::vec3 viewPos = scene.TranslateToViewSpace(lightPosition);
		viewPos = -glm::normalize(viewPos);
		shader.setVector3("lightPositionView", viewPos);
		shader.setInteger("raysOnly", raysOnly);
	}



	GodRaySmooth::GodRaySmooth(GodRay& godRay, GaussianBlur& blur, float resolution)
		: PostProcessingEffect("renderer/postprocessing/combine.frag"), 
			godRay(godRay), blur(blur), resolution(resolution) {
	
		godRay.renderRaysOnly(true);
	}

	GodRaySmooth::~GodRaySmooth() {
		if (rayScreen != nullptr)
			delete rayScreen;
	}


	void GodRaySmooth::setScreen(ScreenQuad& screen) {
		PostProcessingEffect::setScreen(screen);

		rayScreen = new ScreenQuad(screen.width * resolution, screen.height * resolution);
		rayScreen->init();

		rayBuffer.init(rayScreen->width, rayScreen->height);
		blurBuffer.init(rayScreen->width, rayScreen->height);

		godRay.setScreen(*rayScreen);
		blur.setScreen(*rayScreen);
		blur.setParentFBO(blurBuffer.fbo);

		buffers.push_back(blurBuffer.getColorID());
	}


	void GodRaySmooth::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("image2", shader.mapOffset + 1);

		godRay.setBuffer(buffers.front());
		rayBuffer.fill(godRay);

		blur.setBuffer(rayBuffer.getColorID());
		blurBuffer.fill(blur);

		FrameBuffer::resetSize(screen->width, screen->height);
		FrameBuffer::bind(parentFBO);
	}
}