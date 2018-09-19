#define GLEW_STATIC
#include <glew.h>
#include <cassert>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "shadowmapping/shadowmap.h"
#include "transformation/transform.h"
#include "cameras/camera.h"
#include "renderscene.h"
#include "framebuffer/gbuffer.h"
#include "directionallight.h"

using namespace std;
using namespace glm;

namespace geeL {

	DirectionalLight::DirectionalLight(Transform& transform, vec3 diffuse, const string& name)
		: Light(transform, diffuse, name) {}


	void DirectionalLight::bind(const Shader& shader, const string& name, 
		ShaderTransformSpace space, const Camera* const camera) const {
		
		Light::bind(shader, name, space, camera);

		switch (space) {
			case ShaderTransformSpace::View:
				assert(camera != nullptr);

				shader.bind<glm::vec3>(name + "direction", camera->GetOriginInViewSpace() -
					camera->TranslateToViewSpace(transform.getForwardDirection()));
				break;
			case ShaderTransformSpace::World:
				shader.bind<glm::vec3>(name + "direction", transform.getForwardDirection());
				break;
		}
	}

	void DirectionalLight::setMapIndex(unsigned int index, LightMapType type) {
		if (contains(type, LightMapType::Shadow2D)) {
			if (shadowmapIndex != index) shadowmapIndex = index;
		}
	}

	LightMapContainer DirectionalLight::getMaps() const {
		if (shadowMap != nullptr) {
			LightMapContainer container(1);
			container.add(*shadowMap, LightMapType::Shadow2D);

			return container;
		}

		return LightMapContainer();
	}

	const ITexture* const DirectionalLight::getMap(LightMapType type) {
		if (shadowMap && contains(type, LightMapType::Shadow2D))
			return shadowMap;

		return nullptr;
	}

	float DirectionalLight::getAttenuation(glm::vec3 point) const {
		return 1.f;
	}

}