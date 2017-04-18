#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../cameras/camera.h"
#include "../shader/shader.h"
#include "../primitives/screencube.h"
#include "irrmap.h"
#include "cubemap.h"
#include "skybox.h"

namespace geeL {

	Skybox::Skybox(const CubeMap& cubeMap) : cubeMap(cubeMap),
		shader(Shader("renderer/cubemapping/skybox.vert", "renderer/cubemapping/skybox.frag")) {}

	Skybox::Skybox(const IrradianceMap& irradianceMap) 
		: cubeMap(irradianceMap),
			shader(Shader("renderer/cubemapping/skybox.vert", "renderer/cubemapping/skybox.frag")) {}


	void Skybox::draw(const SceneCamera& camera) const {
		glDepthFunc(GL_LEQUAL);
		shader.use();

		mat4 view = mat4(mat3(camera.getViewMatrix()));
		shader.setMat4("view", view);
		shader.setMat4("projection", camera.getProjectionMatrix());
		
		cubeMap.bind(shader, "skybox");

		SCREENCUBE.drawComplete();
		glDepthFunc(GL_LESS);
	}

	void Skybox::bind(Shader& shader) const {
		cubeMap.add(shader, "skybox.");
	}

	unsigned int Skybox::getID() const {
		return cubeMap.getID();
	}

}