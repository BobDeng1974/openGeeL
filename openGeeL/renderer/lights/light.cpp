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
	
		setColorAndIntensityFromDiffuse();
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
			shader.removeMap(*shadowMap);
	}


	float Light::getAttenuation(glm::vec3 point) const {
		float distance = length(transform.getPosition() - point);

		if (distance == 0.f)
			return 0.f;
		
		return 1.f / (distance * distance);
	}

	float Light::getIntensity() const {
		return intensity;
	}

	void Light::setIntensity(float value) {
		if (!transform.isStatic && value > 0.f && value != intensity) {
			intensity = value;
			updateDiffuse();
		}
	}

	vec3 Light::getDiffuse() const {
		return diffuse;
	}

	void Light::setDiffuse(vec3 value) {
		if (!transform.isStatic && !VectorExtension::equals(diffuse, value)) {
			diffuse = value;
			
			setColorAndIntensityFromDiffuse();
			onChange();
		}
	}

	
	vec3 Light::getColor() const {
		return color;
	}

	void Light::setColor(vec3 value) {
		if (!transform.isStatic && !VectorExtension::equals(color, value)) {
			color = clamp(value, 0.f, 1.f);
			updateDiffuse();
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

	float Light::getLightRadius(float cutoff) const {
		float luminance = dot(diffuse, vec3(0.2126f, 0.7152f, 0.0722f));
		return std::sqrtf(luminance / cutoff);
	}

	void Light::updateDiffuse() {
		setDiffuseOnly(color * vec3(intensity));
	}

	void Light::setColorAndIntensityFromDiffuse() {
		intensity = fmax(diffuse.x, fmax(diffuse.y, diffuse.z));
		color = diffuse / intensity;
	}

	void Light::setDiffuseOnly(vec3 value) {
		if (!transform.isStatic && !VectorExtension::equals(diffuse, value)) {
			diffuse = value;
			onChange();
		}
	}

}