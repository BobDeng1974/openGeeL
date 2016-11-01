#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../cameras/camera.h"
#include "cubemap.h"
#include "../shader/shader.h"
#include "skybox.h"

namespace geeL {

	Skybox::Skybox(CubeMap& cubeMap) : cubeMap(cubeMap), 
		shader(Shader("renderer/cubemapping/skybox.vert", "renderer/cubemapping/skybox.frag")) {
	
		initBox();
	}


	void Skybox::draw(const Camera& camera) const {
		glDepthFunc(GL_LEQUAL);
		shader.use();

		mat4 view = mat4(mat3(camera.getViewMatrix()));
		shader.setMat4("view", view);
		shader.setMat4("projection", camera.getProjectionMatrix());
		glBindVertexArray(boxVAO);

		cubeMap.draw(shader, "skybox");
	
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}

	void Skybox::bind(const Shader& shader) const {
		cubeMap.draw(shader, shader.skybox);
	}

	void Skybox::initBox() {

		GLfloat skyboxVertices[] = {        
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		GLuint boxVBO;
		glGenVertexArrays(1, &boxVAO);
		glGenBuffers(1, &boxVBO);
		glBindVertexArray(boxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glBindVertexArray(0);
	}

}