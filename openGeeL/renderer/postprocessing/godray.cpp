#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include "../transformation/transform.h"
#include "../lighting/directionallight.h"
#include "../utility/screenquad.h"
#include "../scene.h"
#include "godray.h"

namespace geeL {

	GodRay::GodRay(const RenderScene& scene, glm::vec3 lightPosition, unsigned int samples)
		: 
		PostProcessingEffect("renderer/postprocessing/godray.frag"), 
		scene(scene), lightPosition(lightPosition), samples(samples) {}


	void GodRay::bindValues() {
		PostProcessingEffect::bindValues();

		shader.setInteger("samples", samples);
		glm::vec3 screenPos = scene.TranslateToScreenSpace(lightPosition);
		shader.setVector3("lightPosition", screenPos);

		glm::vec3 viewPos = scene.TranslateToViewSpace(lightPosition);
		viewPos = -glm::normalize(viewPos);
		shader.setVector3("lightPositionView", viewPos);
	}


}