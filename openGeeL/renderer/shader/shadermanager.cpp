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

	ShaderInformationLinker::ShaderInformationLinker(MaterialFactory& factory) 
		: factory(factory), bindingPointCounter(0) {
	
		camID = generateUniformBuffer(2 * int(sizeof(glm::mat4) + sizeof(glm::vec3)));
	}

	void ShaderInformationLinker::staticBind(const RenderScene& scene, SceneShader& shader) const {
		shader.use();
		
		if (shader.getUseLight()) {
			scene.lightManager.bind(scene, shader);
			scene.lightManager.bindShadowmaps(shader);
		}
		if (shader.getUseCamera()) {
			glUniformBlockBinding(shader.program,
				glGetUniformBlockIndex(shader.program, "cameraMatrices"),
				getUniformBindingPoint(camID));

			scene.getCamera().uniformBind(camID);
		}
	}

	void ShaderInformationLinker::dynamicBind(const RenderScene& scene, SceneShader& shader) const {
		bindCamera(scene);

		shader.use();
		shader.loadMaps();
		if (shader.getUseLight()) scene.lightManager.bind(scene, shader);
		if (shader.getUseCamera()) shader.setViewMatrix(scene.getCamera().getViewMatrix());
	}

	void ShaderInformationLinker::bindCamera(const RenderScene& scene) const {
		scene.getCamera().uniformBind(camID);
	}

	int ShaderInformationLinker::generateUniformBuffer(int size) {
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

	
	int ShaderInformationLinker::getUniformBindingPoint(int id) const {
		if (UBObjects.find(id) == UBObjects.end())
			throw std::invalid_argument("No uniform buffer object with ID: " + id);

		return UBObjects.at(id);
	}

}