#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include "../transformation/transform.h"
#include "../lights/directionallight.h"
#include "../cameras/camera.h"
#include "../primitives/screenquad.h"
#include "../scene.h"
#include "gaussianblur.h"
#include "godray.h"

namespace geeL {

	GodRay::GodRay(glm::vec3 lightPosition, unsigned int samples) 
		: PostProcessingEffectFS("renderer/postprocessing/godray.frag"), 
			lightPosition(lightPosition), samples(samples) {}


	void GodRay::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.set<int>("samples", samples);
		shader.set<int>("raysOnly", onlyEffect);

		lightLocation = shader.getLocation("lightPosition");
		lightViewLocation = shader.getLocation("lightPositionView");
	}

	void GodRay::bindValues() {
		PostProcessingEffectFS::bindValues();

		glm::vec3 screenPos = camera->TranslateToScreenSpace(lightPosition);
		shader.set<glm::vec3>(lightLocation, screenPos);

		glm::vec3 viewPos = camera->TranslateToViewSpace(lightPosition);
		viewPos = -glm::normalize(viewPos);
		shader.set<glm::vec3>(lightViewLocation, viewPos);
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
			shader.set<int>("samples", samples);
		}
	}

}