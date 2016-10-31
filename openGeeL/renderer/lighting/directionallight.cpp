#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "directionallight.h"
#include "../scene.h"

using namespace std;
using namespace glm;

namespace geeL {

	DirectionalLight::DirectionalLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, 
		float intensity, float shadowBias)
		: 
		Light(transform, diffuse, specular, ambient, intensity, shadowBias) {}


	void DirectionalLight::deferredBind(const Shader& shader, int index, string name) const {
		Light::deferredBind(shader, index, name);

		string location = name + "[" + to_string(index) + "].";
		shader.setVector3(location + "direction", transform.forward);
		shader.setMat4(location + "lightTransform", lightTransform);
	}

	void DirectionalLight::forwardBind(const Shader& shader, int index, string name) const {
		Light::forwardBind(shader, index, name);

		string location = name + "[" + to_string(index) + "].";
		shader.setVector3(location + "direction", transform.forward);

		location = "direLightMatrix[" + to_string(index) + "]";
		shader.setMat4(location, lightTransform);
	}

	void DirectionalLight::computeLightTransform() {
		float a = shadowmapWidth / 100.f;
		mat4 projection = ortho(-a, a, -a, a, 1.0f, 50.f);
		mat4 view = lookAt(transform.forward, vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f));
		
		lightTransform = projection * view;
	}
}