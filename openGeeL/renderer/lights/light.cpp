#define GLEW_STATIC
#include <glew.h>
#include <string>
#include "../shader/shader.h"
#include "../shader/sceneshader.h"
#include "../cameras/camera.h"
#include "../transformation/transform.h"
#include "../scene.h"
#include "../shadowmapping/shadowmap.h"
#include "../utility/vectorextension.h"
#include "light.h"

using namespace std;
using namespace glm;

namespace geeL {

	Light::Light(Transform& transform, vec3 diffuse, const std::string& name)
		: SceneObject(transform, name), diffuse(diffuse), shadowMap(nullptr) {}

	Light::~Light() {
		if (shadowMap != nullptr)
			delete shadowMap;
	}


	const ShadowMap* const Light::getShadowMap() const {
		return shadowMap;
	}

	ShadowMap* const Light::getShadowMap() {
		return shadowMap;
	}

	void Light::setShadowMap(ShadowMap& map) {
		shadowMap = &map;
	}

	void Light::bind(const Camera& camera, const Shader& shader, const std::string& name, ShaderTransformSpace space) const {
		shader.setVector3(name + "diffuse", diffuse);

		if (shadowMap != nullptr)
			shadowMap->bindData(shader, name);
		else
			shader.setInteger(name + "type", 0);
	}
			

	void Light::bind(const Camera& camera, const SceneShader& shader, const std::string& name) const {
		bind(camera, shader, name, shader.getSpace());
	}

	void Light::addShadowmap(Shader& shader, const std::string& name) {
		if (shadowMap != nullptr)
			shadowMap->bindMap(shader, name);
	}

	void Light::removeShadowmap(Shader& shader) {
		if (shadowMap != nullptr)
			shadowMap->removeMap(shader);
	}

	void Light::renderShadowmap(const SceneCamera* const camera, 
		std::function<void(const Shader&)> renderCall, const Shader& shader) {

		if (shadowMap != nullptr && !transform.isStatic)
			shadowMap->draw(camera, renderCall, shader);
	}

	void Light::renderShadowmapForced(const SceneCamera* const camera,
		std::function<void(const Shader&)> renderCall, const Shader& shader) {

		if (shadowMap != nullptr)
			shadowMap->draw(camera, renderCall, shader);
	}

	float Light::getIntensity(glm::vec3 point) const {
		float distance = length(transform.getPosition() - point);

		if (distance == 0.f)
			return 0.f;
		
		return 1.f / (distance * distance);
	}

	vec3 Light::getColor() const {
		return diffuse;
	}

	void Light::setColor(vec3 color) {
		if (!transform.isStatic && !VectorExtension::equals(diffuse, color))
			diffuse = color;
	}

}