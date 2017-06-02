#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "../shader/rendershader.h"
#include "../shader/sceneshader.h"
#include "../transformation/transform.h"
#include "../cameras/camera.h"
#include "../scene.h"
#include "../framebuffer/gbuffer.h"
#include "directionallight.h"
#include <iostream>

using namespace std;
using namespace glm;

namespace geeL {

	DirectionalLight::DirectionalLight(Transform& transform, vec3 diffuse, const string& name)
		: Light(transform, diffuse, name) {}


	void DirectionalLight::bind(const Camera& camera, const RenderShader& shader,
		const string& name, ShaderTransformSpace space) const {
		Light::bind(camera, shader, name, space);

		switch (space) {
			case ShaderTransformSpace::View:
				shader.setVector3(name + "direction", camera.GetOriginInViewSpace() -
					camera.TranslateToViewSpace(transform.getForwardDirection()));
				break;
			case ShaderTransformSpace::World:
				shader.setVector3(name + "direction", transform.getForwardDirection());
				break;
		}
	}

	float DirectionalLight::getIntensity(glm::vec3 point) const {
		return 1.f;
	}

}