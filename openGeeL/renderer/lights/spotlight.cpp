#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <cassert>
#include <gtc/matrix_transform.hpp>
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
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
			, angle(angle)
			, outerAngle(outerAngle)
			, cutoff(cutoff)
			, lightCookie(nullptr) {}


	void SpotLight::bind(const Shader& shader, const string& name, ShaderTransformSpace space, 
		const Camera* const camera) const {
		
		Light::bind(shader, name, space, camera);
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
		shader.bind<int>(name + "useCookie", (lightCookie != nullptr));
	}

	void SpotLight::setLightCookie(memory::MemoryObject<ImageTexture> cookie) {
		lightCookie = cookie;

		//Force wrap mode to avoid artifacts with wrap methods like 'Repeat'
		lightCookie->setWrapMode(WrapMode::ClampBorder);
	}

	const ITexture* const SpotLight::getLightCookie() const {
		return lightCookie.get();
	}

	void SpotLight::addShadowmap(Shader& shader, const std::string& name) {
		Light::addShadowmap(shader, name);

		addLightCookie(shader, name);
	}

	void SpotLight::addLightCookie(Shader& shader, const string& name) {
		if(lightCookie != nullptr)
			shader.addMap(*lightCookie, name + "cookie");
	}


}