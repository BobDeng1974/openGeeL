#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "../cameras/camera.h"
#include "../scene.h"
#include "../utility/gbuffer.h"
#include "directionallight.h"
#include <iostream>

using namespace std;
using namespace glm;

namespace geeL {

	DirectionalLight::DirectionalLight(Transform& transform, vec3 diffuse, const string& name)
		: Light(transform, diffuse, name) {}


	void DirectionalLight::deferredBind(const RenderScene& scene, const Shader& shader, const string& name) const {
		Light::deferredBind(scene, shader, name);

		shader.setVector3(name + "direction", scene.GetOriginInViewSpace() - 
			scene.TranslateToViewSpace(transform.getForwardDirection()));
	}

	void DirectionalLight::forwardBind(const Shader& shader, const string& name, const string& transformName) const {
		Light::forwardBind(shader, name, transformName);

		shader.setVector3(name + "direction", transform.getForwardDirection());
	}

	float DirectionalLight::getIntensity(glm::vec3 point) const {
		return 1.f;
	}

}