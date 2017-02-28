#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../cameras/camera.h"
#include "cubemap.h"
#include "../shader/shader.h"
#include "../primitives/screencube.h"
#include "skybox.h"

namespace geeL {

	Skybox::Skybox(CubeMap& cubeMap) : cubeMap(cubeMap), 
		shader(Shader("renderer/cubemapping/skybox.vert", "renderer/cubemapping/skybox.frag")) {

	}


	void Skybox::draw(const Camera& camera) const {
		glDepthFunc(GL_LEQUAL);
		shader.use();

		mat4 view = mat4(mat3(camera.getViewMatrix()));
		shader.setMat4("view", view);
		shader.setMat4("projection", camera.getProjectionMatrix());
		

		cubeMap.draw(shader, "skybox");

		SCREENCUBE.drawComplete();
		glDepthFunc(GL_LESS);
	}

	void Skybox::bind(const Shader& shader) const {
		cubeMap.draw(shader, shader.skybox);
	}

	unsigned int Skybox::getID() const {
		return cubeMap.getID();
	}

}