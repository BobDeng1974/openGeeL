#define GLEW_STATIC
#include <glew.h>
#include <cassert>
#include <algorithm>
#include <gtc/matrix_transform.hpp>
#include <string>
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "shadowmapping/shadowmap.h"
#include "transformation/transform.h"
#include "cameras/camera.h"
#include "renderscene.h"
#include "pointlight.h"

using namespace std;
using namespace glm;

namespace geeL {

	PointLight::PointLight(Transform& transform, vec3 diffuse, float cutoff, const string& name)
		: Light(transform, diffuse, name)
		, cutoff(cutoff)
		, volStrength(0.f)
		, volDensity(1.f) {}


	void PointLight::bind(const Shader& shader, const string& name, 
		ShaderTransformSpace space, const Camera* const camera) const {
		
		Light::bind(shader, name, space, camera);
		shader.bind<float>(name + "radius", getLightRadius(cutoff));
		shader.bind<float>(name + "volumetricStrength", volStrength);
		shader.bind<float>(name + "volumetricDensity", volDensity);

		switch (space) {
			case ShaderTransformSpace::View:
				assert(camera != nullptr);

				shader.bind<glm::vec3>(name + "position", camera->TranslateToViewSpace(transform.getPosition()));
				break;
			case ShaderTransformSpace::World:
				shader.bind<glm::vec3>(name + "position", transform.getPosition());
				break;
		}
		
	}


	void PointLight::setMapIndex(unsigned int index, LightMapType type) {
		if (contains(type, LightMapType::ShadowCube)) {
			if (shadowmapIndex != index) shadowmapIndex = index;
		}
	}

	LightMapContainer PointLight::getMaps() const {
		if (shadowMap != nullptr) {
			LightMapContainer container(1);
			container.add(*shadowMap, LightMapType::ShadowCube);

			return container;
		}

		return LightMapContainer();
	}

	const ITexture* const PointLight::getMap(LightMapType type) {
		if (shadowMap && contains(type, LightMapType::ShadowCube))
			return shadowMap;

		return nullptr;
	}

	float PointLight::getVolumetricStrength() const {
		return volStrength;
	}

	float PointLight::getVolumetricDensity() const {
		return volDensity;
	}

	void PointLight::setVolumetricStrength(float value) {
		if (value >= 0.f && volStrength != value) {
			volStrength = value;
		}
	}

	void PointLight::setVolumetricDensity(float value) {
		if (value >= 0.f && volDensity != value) {
			volDensity = value;
		}
	}

}