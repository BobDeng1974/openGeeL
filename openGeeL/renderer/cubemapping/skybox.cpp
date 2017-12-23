#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "cameras/camera.h"
#include "shader/rendershader.h"
#include "primitives/screencube.h"
#include "irrmap.h"
#include "cubemap.h"
#include "skybox.h"

namespace geeL {

	Skybox::Skybox(const CubeMap& cubeMap, float brightnessScale) 
		: cubeMap(cubeMap)
		, shader(RenderShader("shaders/cubemapping/skybox.vert", 
			"shaders/cubemapping/skybox.frag")) {
	
		setBrightnessScale(brightnessScale);
	}


	void Skybox::draw(const Camera& camera) const {
		glDepthFunc(GL_LEQUAL);

		mat4 view = mat4(mat3(camera.getViewMatrix()));
		shader.bind<glm::mat4>("view", view);
		camera.bindProjectionMatrix(shader, "projection");

		cubeMap.bindMap(shader, "skybox");

		SCREENCUBE.drawComplete();
		glDepthFunc(GL_LESS);
	}

	void Skybox::bind(RenderShader& shader) const {
		cubeMap.add(shader, "skybox.");
	}

	float Skybox::getBrightnessScale() const {
		return brightnessScale;
	}

	void Skybox::setBrightnessScale(float value) {
		if (value > 0.f && value != brightnessScale) {
			brightnessScale = value;
			shader.bind<float>("brightnessScale", value);
		}
	}

}