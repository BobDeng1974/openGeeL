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

	Light::Light(Transform& transform, vec3 diffuse, const string& name)
		: SceneObject(transform, name)
		, diffuse(diffuse)
		, shadowMap(nullptr)
		, shadowmapIndex(0) {
	
		setColorAndIntensityFromDiffuse();
		transform.addListener(*this);
	}

	Light::~Light() {
		transform.removeListener(*this);

		if (shadowMap != nullptr)
			delete shadowMap;
	}


	const Shadowmap* const Light::getShadowMap() const {
		return shadowMap;
	}

	Shadowmap* Light::getShadowMap() {
		return shadowMap;
	}


	void Light::attachShadowmap(unique_ptr<Shadowmap> map) {
		if (shadowMap != nullptr)
			delete shadowMap;

		shadowMap = map.release();
		onMapChange();
	}

	void Light::detatchShadowmap() {
		if (shadowMap != nullptr) {
			delete shadowMap;
			shadowMap = nullptr;

			onMapChange();
		}
	}

	void Light::bind(const Shader& shader, const std::string& name, ShaderTransformSpace space, const Camera* const camera) const {
		shader.bind<unsigned int>(name + "shadowmapIndex", shadowmapIndex);
		shader.bind<glm::vec3>(name + "diffuse", diffuse);

		if (shadowMap != nullptr)
			shadowMap->bindData(shader, name);
		else
			shader.bind<int>(name + "type", 0);
	}
			

	void Light::bind(const SceneShader& shader, const std::string& name, const Camera* const camera) const {
		bind(shader, name, shader.getSpace(), camera);
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
			onChange(transform);
		}
	}

	
	vec3 Light::getColor() const {
		return color;
	}

	bool Light::hasActiveMaps() const {
		return isActive() && getMaps().size() > 0;
	}

	void Light::setActive(bool active) {
		bool a = isActive();
		SceneObject::setActive(active);

		if (a != active && !transform.isStatic)
			onMapChange();
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

	void Light::addShadowmapChangeListener(std::function<void(Light&)> function, bool invoke) {
		changeMapListeners.push_back(function);

		if (hasActiveMaps()) function(*this);
	}

	void Light::onChange(const Transform& t) {
		for (auto it(changeListeners.begin()); it != changeListeners.end(); it++) {
			auto function = *it;
			function(*this);
		}
	}

	void Light::onMapChange() {
		for (auto it(changeMapListeners.begin()); it != changeMapListeners.end(); it++) {
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
			onChange(transform);
		}
	}

}