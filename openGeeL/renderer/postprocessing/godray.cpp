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
			lightPosition(lightPosition) {
	
		shader.setValue("samples", samples, Range<int>(0, 200));
	}


	void GodRay::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.bind<int>("raysOnly", onlyEffect);
		lightLocation = shader.getLocation("lightPosition");
		lightViewLocation = shader.getLocation("lightPositionView");
	}

	void GodRay::bindValues() {
		PostProcessingEffectFS::bindValues();

		glm::vec3 screenPos = camera->TranslateToScreenSpace(lightPosition);
		shader.bind<glm::vec3>(lightLocation, screenPos);

		glm::vec3 viewPos = camera->TranslateToViewSpace(lightPosition);
		viewPos = -glm::normalize(viewPos);
		shader.bind<glm::vec3>(lightViewLocation, viewPos);
	}

	glm::vec3 GodRay::getLightPosition() const {
		return lightPosition;
	}

	void GodRay::setLightPosition(glm::vec3 position) {
		lightPosition = position;
	}

	unsigned int GodRay::getSampleCount() const {
		return shader.getIntValue("samples");
	}

	void GodRay::setSampleCount(unsigned int samples) {
		shader.setValue("samples", samples, Range<int>(0, 200));
	}

}