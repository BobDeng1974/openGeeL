#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "../shader/rendershader.h"
#include "../shader/sceneshader.h"
#include "../texturing/imagetexture.h"
#include "../transformation/transform.h"
#include "../cameras/camera.h"
#include "../scene.h"
#include "spotlight.h"

using namespace std;
using namespace glm;


namespace geeL {

	SpotLight::SpotLight(Transform& transform, vec3 diffuse, float angle, float outerAngle, const string& name)
			: Light(transform, diffuse, name), angle(angle), outerAngle(outerAngle), lightCookie(nullptr) {}


	void SpotLight::bind(const Camera& camera, const RenderShader& shader, const string& name, ShaderTransformSpace space) const {
		Light::bind(camera, shader, name, space);

		switch (space) {
			case ShaderTransformSpace::View:
				shader.bind<glm::vec3>(name + "position", camera.TranslateToViewSpace(transform.getPosition()));
				shader.bind<glm::vec3>(name + "direction",
					camera.TranslateToViewSpace(transform.getForwardDirection()) - camera.GetOriginInViewSpace());
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

	void SpotLight::setLightCookie(ImageTexture& cookie) {
		lightCookie = &cookie;

		//Force wrap mode to avoid artifacts with wrap methods like 'Repeat'
		cookie.bind();
		cookie.initWrapMode(WrapMode::ClampBorder);
	}

	const Texture * const SpotLight::getLightCookie() const {
		return lightCookie;
	}

	void SpotLight::addLightCookie(RenderShader& shader, const string& name) {
		if(lightCookie != nullptr)
			shader.addMap(*lightCookie, name);
	}


}