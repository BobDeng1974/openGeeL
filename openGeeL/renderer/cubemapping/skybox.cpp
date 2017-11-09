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

	Skybox::Skybox(const CubeMap& cubeMap) : cubeMap(cubeMap),
		shader(RenderShader("renderer/cubemapping/skybox.vert", "renderer/cubemapping/skybox.frag")) {}


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

	unsigned int Skybox::getID() const {
		return cubeMap.getID();
	}

}