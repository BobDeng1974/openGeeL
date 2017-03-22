#define GLEW_STATIC
#include <glew.h>
#include <list>
#include "sceneshader.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"
#include "../materials/materialfactory.h"
#include "shadermanager.h"
#include "../scene.h"


namespace geeL {

	ShaderManager::ShaderManager(MaterialFactory& factory) 
		: factory(factory), bindingPointCounter(0) {
	
		camID = generateUniformBuffer(2 * int(sizeof(glm::mat4) + sizeof(glm::vec3)));
	}

	void ShaderManager::staticBind(const RenderScene& scene) const {
		for (auto it = factory.shadersBegin(); it != factory.shadersEnd(); it++) {
			SceneShader& shader = **it;
			staticForwardBind(scene, shader);
		}
	}

	void ShaderManager::staticForwardBind(const RenderScene& scene, SceneShader& shader) const {
		shader.use();

		if (shader.getUseLight()) {
			scene.lightManager.forwardBind(shader);
			shader.mapOffset = 1;
			scene.lightManager.bindShadowmaps(shader);
		}
		if (shader.getUseCamera()) {
			glUniformBlockBinding(shader.program,
				glGetUniformBlockIndex(shader.program, "cameraMatrices"),
				getUniformBindingPoint(camID));

			scene.camera.uniformBind(camID);
		}

		shader.bindMaps();
	}

	void ShaderManager::staticDeferredBind(const RenderScene& scene, Shader& shader) const {
		shader.use();

		scene.lightManager.deferredBind(scene, shader);
		shader.mapOffset = 1;
		scene.lightManager.bindShadowmaps(shader);

		glUniformBlockBinding(shader.program,
			glGetUniformBlockIndex(shader.program, "cameraMatrices"),
			getUniformBindingPoint(camID));

		scene.camera.uniformBind(camID);

		shader.bindMaps();
	}

	void ShaderManager::dynamicBind(const RenderScene& scene) const {
		bindCamera(scene);

		for (auto it = factory.shadersBegin(); it != factory.shadersEnd(); it++) {
			const SceneShader& shader = **it;

			shader.use();
			if (shader.getUseLight()) scene.lightManager.forwardBind(shader);
		}
	}

	void ShaderManager::bindCamera(const RenderScene& scene) const {
		scene.camera.uniformBind(camID);
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