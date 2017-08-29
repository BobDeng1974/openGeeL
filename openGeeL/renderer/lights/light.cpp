#define GLEW_STATIC
#include <glew.h>
#include <string>
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "cameras/camera.h"
#include "transformation/transform.h"
#include "renderscene.h"
#include "shadowmapping/shadowmap.h"
#include "utility/vectorextension.h"
#include "light.h"

using namespace std;
using namespace glm;

namespace geeL {

	Light::Light(Transform& transform, vec3 diffuse, const std::string& name)
		: SceneObject(transform, name), diffuse(diffuse), shadowMap(nullptr) {
	
		transform.addChangeListener([this](const Transform& transform) { onChange(); });
	}

	Light::~Light() {
		if (shadowMap != nullptr)
			delete shadowMap;
	}


	const ShadowMap* const Light::getShadowMap() const {
		return shadowMap;
	}

	ShadowMap* Light::getShadowMap() {
		return shadowMap;
	}

	void Light::setShadowMap(ShadowMap& map) {
		shadowMap = &map;
	}

	void Light::bind(const Shader& shader, const std::string& name, ShaderTransformSpace space, const Camera* const camera) const {
		shader.bind<glm::vec3>(name + "diffuse", diffuse);

		if (shadowMap != nullptr)
			shadowMap->bindData(shader, name);
		else
			shader.bind<int>(name + "type", 0);
	}
			

	void Light::bind(const SceneShader& shader, const std::string& name, const Camera* const camera) const {
		bind(shader, name, shader.getSpace(), camera);
	}

	void Light::addShadowmap(Shader& shader, const std::string& name) {
		if (shadowMap != nullptr)
			shader.addMap(*shadowMap, name + "shadowMap");
	}

	void Light::removeShadowmap(Shader& shader) {
		if (shadowMap != nullptr)
			shadowMap->removeMap(shader);
	}

	void Light::renderShadowmap(const SceneCamera* const camera, 
		std::function<void(const RenderShader&)> renderCall, const ShadowmapRepository& repository) {

		if (shadowMap != nullptr && !transform.isStatic)
			shadowMap->draw(camera, renderCall, repository);
	}

	void Light::renderShadowmapForced(const SceneCamera* const camera,
		std::function<void(const RenderShader&)> renderCall, const ShadowmapRepository& repository) {

		if (shadowMap != nullptr)
			shadowMap->draw(camera, renderCall, repository);
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
		if (!transform.isStatic && !VectorExtension::equals(diffuse, color)) {
			diffuse = color;
			onChange();
		}
	}


	void Light::addChangeListener(std::function<void(Light&)> function, bool invoke) {
		changeListeners.push_back(function);

		if (invoke) function(*this);
	}

	void Light::onChange() {
		for (auto it(changeListeners.begin()); it != changeListeners.end(); it++) {
			auto function = *it;
			function(*this);
		}
	}

}