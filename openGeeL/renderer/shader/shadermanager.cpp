#define GLEW_STATIC
#include <glew.h>
#include <list>
#include "shader.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"
#include "../materials/materialfactory.h"
#include "shadermanager.h"
#include "../scene.h"


namespace geeL {

	ShaderManager::ShaderManager(MaterialFactory& factory) 
		: factory(factory), bindingPointCounter(0) {
	
		camID = generateUniformBuffer(2.f * sizeof(glm::mat4) + sizeof(glm::vec3));
	}

	void ShaderManager::staticBind(const RenderScene& scene) const {
		for (list<Shader*>::iterator it = factory.shadersBegin(); it != factory.shadersEnd(); it++) {
			Shader& shader = **it;
			staticBind(scene, shader);
		}
	}

	void ShaderManager::staticBind(const RenderScene& scene, Shader& shader) const {
		shader.use();

		if (shader.useLight) {
			scene.lightManager.bind(shader);
			scene.lightManager.bindShadowmaps(shader);
		}
		if (shader.useSkybox) scene.bindSkybox(shader);
		if (shader.useCamera) {
			glUniformBlockBinding(shader.program,
				glGetUniformBlockIndex(shader.program, "cameraMatrices"),
				getUniformBindingPoint(camID));

			scene.camera.uniformBind(camID);
		}
	}

	void ShaderManager::dynamicBind(const RenderScene& scene) const {
		scene.camera.uniformBind(camID);

		for (list<Shader*>::const_iterator it = factory.shadersBegin(); it != factory.shadersEnd(); it++) {
			const Shader& shader = **it;

			shader.use();
			if (shader.useLight) scene.lightManager.bind(shader);
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