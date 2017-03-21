#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include "../transformation/transform.h"
#include "../lighting/directionallight.h"
#include "../primitives/screenquad.h"
#include "../scene.h"
#include "gaussianblur.h"
#include "godray.h"

namespace geeL {

	GodRay::GodRay(const RenderScene& scene, glm::vec3 lightPosition, unsigned int samples) 
		: PostProcessingEffect("renderer/postprocessing/godray.frag"), 
			scene(scene), lightPosition(lightPosition), samples(samples) {}


	void GodRay::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		shader.setInteger("samples", samples);
		shader.setInteger("raysOnly", onlyEffect);
	}

	void GodRay::bindValues() {
		PostProcessingEffect::bindValues();

		glm::vec3 screenPos = scene.TranslateToScreenSpace(lightPosition);
		shader.setVector3("lightPosition", screenPos);

		glm::vec3 viewPos = scene.TranslateToViewSpace(lightPosition);
		viewPos = -glm::normalize(viewPos);
		shader.setVector3("lightPositionView", viewPos);
	}

	glm::vec3 GodRay::getLightPosition() const {
		return lightPosition;
	}

	void GodRay::setLightPosition(glm::vec3 position) {
		lightPosition = position;
	}

	unsigned int GodRay::getSampleCount() const {
		return samples;
	}

	void GodRay::setSampleCount(unsigned int samples) {
		if (samples < 200 && samples != this->samples) {
			this->samples = samples;

			shader.use();
			shader.setInteger("samples", samples);
		}
	}

}