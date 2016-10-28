#define GLEW_STATIC
#include <glew.h>
#include "../transformation/transform.h"
#include "../lighting/directionallight.h"
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
		glm::vec3 pos = scene.TranslateToScreenSpace(lightPosition);
		glUniform3f(glGetUniformLocation(shader.program, "lightPosition"), pos.x, pos.y, pos.z);
	}


}