#define GLEW_STATIC
#include <glew.h>
#include <list>
#include "shader/sceneshader.h"
#include "cameras/camera.h"
#include "lights/lightmanager.h"
#include "materials/materialfactory.h"
#include "transformation/transform.h"
#include "scene.h"
#include "uniformstack.h"

namespace geeL {

	UniformBindingStack::UniformBindingStack()
		: bindingPointCounter(0) {
	
		camID = generateUniformBuffer(2 * int(sizeof(glm::mat4) + sizeof(glm::vec3)));
	}

	void UniformBindingStack::staticBind(const Camera& camera, SceneShader& shader) const {
		if (shader.getUseCamera()) {
			glUniformBlockBinding(shader.getProgram(),
				glGetUniformBlockIndex(shader.getProgram(), "cameraMatrices"),
				getUniformBindingPoint(camID));

			camera.uniformBind(camID);
		}
	}


	void UniformBindingStack::bindCamera(const Camera& camera) const {
		camera.uniformBind(camID);
	}

	int UniformBindingStack::generateUniformBuffer(int size) {
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

	
	int UniformBindingStack::getUniformBindingPoint(int id) const {
		if (UBObjects.find(id) == UBObjects.end())
			throw std::invalid_argument("No uniform buffer object with ID: " + id);

		return UBObjects.at(id);
	}

}