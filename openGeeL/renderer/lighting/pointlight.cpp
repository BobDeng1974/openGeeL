#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <string>
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "../meshes/meshrenderer.h"
#include "../cameras/camera.h"
#include "../scene.h"
#include "pointlight.h"

using namespace std;
using namespace glm;

namespace geeL {

	PointLight::PointLight(Transform& transform, vec3 diffuse, const string& name)
		: Light(transform, diffuse, name) {}


	void PointLight::deferredBind(const RenderScene& scene, const Shader& shader, const string& name) const {
		Light::deferredBind(scene, shader, name);

		shader.setVector3(name + "position", scene.TranslateToViewSpace(transform.getPosition()));
	}

	void PointLight::forwardBind(const Shader& shader, const string& name, const string& transformName) const {
		Light::forwardBind(shader, name, transformName);

		shader.setVector3(name + "position", transform.getPosition());
	}

}