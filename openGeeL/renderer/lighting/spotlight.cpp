#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "../scene.h"
#include "spotlight.h"
#include <iostream>

using namespace std;
using namespace glm;


namespace geeL {

	SpotLight::SpotLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, float intensity, float angle, 
		float outerAngle, float shadowBias, float constant, float linear, float quadratic)
		: 
		Light(transform, diffuse, specular, ambient, intensity, shadowBias),
		angle(angle), outerAngle(outerAngle),
		constant(constant), linear(linear), quadratic(quadratic) {}


	void SpotLight::deferredBind(const RenderScene& scene, const Shader& shader, int index, string name) const {
		Light::deferredBind(scene, shader, index, name);

		string location = name + "[" + to_string(index) + "].";
		shader.setVector3(location + "position", scene.TranslateToViewSpace(transform.position));
		shader.setVector3(location + "direction", 
			scene.TranslateToViewSpace(transform.forward) - scene.GetOriginInViewSpace());
		shader.setFloat(location + "angle", angle);
		shader.setFloat(location + "outerAngle", outerAngle);
		shader.setFloat(location + "constant", constant);
		shader.setFloat(location + "linear", linear);
		shader.setFloat(location + "quadratic", quadratic);
		shader.setMat4(location + "lightTransform", lightTransform);
	}

	void SpotLight::forwardBind(const Shader& shader, int index, string name) const {
		Light::forwardBind(shader, index, name);

		string location = name + "[" + to_string(index) + "].";
		shader.setVector3(location + "position", transform.position);
		shader.setVector3(location + "direction", transform.forward);
		shader.setFloat(location + "angle", angle);
		shader.setFloat(location + "outerAngle", outerAngle);
		shader.setFloat(location + "constant", constant);
		shader.setFloat(location + "linear", linear);
		shader.setFloat(location + "quadratic", quadratic);

		location = "spotLightMatrix[" + to_string(index) + "]";
		shader.setMat4(location, lightTransform);
	}

	void SpotLight::computeLightTransform() {
		float fov = glm::degrees(angle);
		mat4 projection = glm::perspective(fov, 1.f, 1.0f, 50.f);
		mat4 view = lookAt(transform.position, transform.position + transform.forward, transform.up);

		lightTransform = projection * view;
	}
}