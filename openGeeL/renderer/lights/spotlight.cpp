#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <string>
#include <cassert>
#include <gtc/matrix_transform.hpp>
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "shadowmapping/shadowmap.h"
#include "texturing/imagetexture.h"
#include "transformation/transform.h"
#include "cameras/camera.h"
#include "renderscene.h"
#include "spotlight.h"

using namespace std;
using namespace glm;


namespace geeL {

	SpotLight::SpotLight(Transform& transform, 
		vec3 diffuse, 
		float angle, 
		float outerAngle, 
		float cutoff,
		const string& name)
			: Light(transform, diffuse, name)
			, angle(glm::cos(glm::radians(angle)))
			, outerAngle(glm::cos(glm::radians(angle + outerAngle)))
			, cutoff(cutoff)
			, lightCookie(nullptr)
			, cookieIndex(0) {}


	void SpotLight::bind(const Shader& shader, const string& name, ShaderTransformSpace space, 
		const Camera* const camera) const {
		
		Light::bind(shader, name, space, camera);
		shader.bind<unsigned int>(name + "cookieIndex", cookieIndex);
		shader.bind<float>(name + "radius", getLightRadius(cutoff));

		switch (space) {
			case ShaderTransformSpace::View:
				assert(camera != nullptr);

				shader.bind<glm::vec3>(name + "position", camera->TranslateToViewSpace(transform.getPosition()));
				shader.bind<glm::vec3>(name + "direction",
					camera->TranslateToViewSpace(transform.getForwardDirection()) - camera->GetOriginInViewSpace());
				break;
			case ShaderTransformSpace::World:
				shader.bind<glm::vec3>(name + "position", transform.getPosition());
				shader.bind<glm::vec3>(name + "direction", transform.getForwardDirection());
				break;
		}

		shader.bind<float>(name + "angle", angle);
		shader.bind<float>(name + "outerAngle", outerAngle);
	}

	void SpotLight::setMapIndex(unsigned int index, LightMapType type) {
		if (contains(type, LightMapType::Shadow2D)) {
			if (shadowmapIndex != index) shadowmapIndex = index;
		}
		else if (contains(type, LightMapType::Cookie)) {
			if (cookieIndex != index) cookieIndex = index;
		}
	}

	LightMapContainer SpotLight::getMaps() const {
		LightMapContainer container;

		if (shadowMap != nullptr) container.add(*shadowMap, LightMapType::Shadow2D);
		if (lightCookie != nullptr) container.add(*lightCookie, LightMapType::Cookie);

		return container;
	}

	const ITexture* const SpotLight::getMap(LightMapType type){
		if (shadowMap && contains(type, LightMapType::Shadow2D))
			return shadowMap;
		if (lightCookie && contains(type, LightMapType::Cookie))
			return lightCookie.get();

		return nullptr;
	}

	void SpotLight::setLightCookie(memory::MemoryObject<ImageTexture> cookie) {
		lightCookie = cookie;

		//Force wrap mode to avoid artifacts with wrap methods like 'Repeat'
		lightCookie->setWrapMode(WrapMode::ClampBorder);

		onMapChange();
	}

	const ITexture* const SpotLight::getLightCookie() const {
		return lightCookie.get();
	}


	float SpotLight::getAngle() const {
		return angle;
	}

	float SpotLight::getAngleDegree() const {
		return glm::degrees(glm::acos(angle));
	}

	void SpotLight::setAngleDegree(float value) {
		float val = glm::cos(glm::radians(value));

		if (!transform.isStatic && value > 0.f && value < 180.f && val != angle) {
			angle = val;
			outerAngle = glm::cos(glm::radians(value + outerAngle));
		}
	}

	float SpotLight::getOuterAngleDegree() const {
		return glm::degrees(glm::acos(outerAngle)) - getAngleDegree();
	}

	void SpotLight::setOuterAngleDegree(float value) {
		float val = glm::cos(glm::radians(value + getAngleDegree()));

		if (!transform.isStatic && value > 0.f && value < 15.f && val != outerAngle) {
			outerAngle = val;
		}
	}

}