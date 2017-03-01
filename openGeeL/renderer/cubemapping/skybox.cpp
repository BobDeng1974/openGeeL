#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../cameras/camera.h"
#include "../shader/shader.h"
#include "../primitives/screencube.h"
#include "cubemap.h"
#include "skybox.h"

namespace geeL {

	Skybox::Skybox(CubeMap& cubeMap) : cubeMap(cubeMap), irradianceMap(nullptr),
		shader(Shader("renderer/cubemapping/skybox.vert", "renderer/cubemapping/skybox.frag")) {}


	void Skybox::draw(const Camera& camera) const {
		glDepthFunc(GL_LEQUAL);
		shader.use();

		mat4 view = mat4(mat3(camera.getViewMatrix()));
		shader.setMat4("view", view);
		shader.setMat4("projection", camera.getProjectionMatrix());
		
		cubeMap.bind(shader, "skybox");

		SCREENCUBE.drawComplete();
		glDepthFunc(GL_LESS);
	}

	void Skybox::addIrradianceMap(CubeMap& irradianceMap) {
		this->irradianceMap = &irradianceMap;
	}

	void Skybox::bind(Shader& shader) const {
		cubeMap.add(shader, "skybox");

		if (irradianceMap != nullptr)
			irradianceMap->add(shader, "irradianceMap");
	}

	unsigned int Skybox::getID() const {
		return cubeMap.getID();
	}

}