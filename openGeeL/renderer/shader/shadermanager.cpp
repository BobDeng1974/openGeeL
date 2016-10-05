#define GLEW_STATIC
#include <glew.h>
#include <list>
#include "shader.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"
#include "../materials/materialfactory.h"
#include "shadermanager.h"

#include <iostream>

namespace geeL {

	ShaderManager::ShaderManager(const MaterialFactory& factory) 
		: factory(factory), bindingPointCounter(0) {
	
		camID = generateUniformBuffer(2.f * sizeof(glm::mat4) + sizeof(glm::vec3));
	}


	void ShaderManager::staticBind(const LightManager& lightManager, const Camera& currentCamera) const {

		for (list<Shader>::const_iterator it = factory.shadersBegin(); it != factory.shadersEnd(); it++) {
			const Shader& shader = *it;

			shader.use();
			if (shader.useLight) lightManager.bind(shader);
			if (shader.useSkybox) currentCamera.bindSkybox(shader);
			if (shader.useCamera) glUniformBlockBinding(shader.program,
				glGetUniformBlockIndex(shader.program, "cameraMatrices"),
				getUniformBindingPoint(camID));
		}
	}

	void ShaderManager::dynamicBind(const LightManager& lightManager, const Camera& currentCamera) const {
		currentCamera.uniformBind(camID);

		for (list<Shader>::const_iterator it = factory.shadersBegin(); it != factory.shadersEnd(); it++) {
			const Shader& shader = *it;

			shader.use();
			if (shader.useLight) lightManager.bind(shader);
		}
	}

	int ShaderManager::generateUniformBuffer(int size) {
		GLuint ubo;
		glGenBuffers(1, &ubo);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, bindingPointCounter, ubo, 0, size);

		UBObjects[ubo] = bindingPointCounter;
		bindingPointCounter++;
		
		return ubo;
	}

	
	int ShaderManager::getUniformBindingPoint(int id) const {
		if (UBObjects.find(id) == UBObjects.end())
			throw std::invalid_argument("No uniform buffer object with ID: " + id);

		return UBObjects.at(id);
	}

}