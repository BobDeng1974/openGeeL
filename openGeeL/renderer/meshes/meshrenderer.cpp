#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../materials/material.h"
#include "../transformation/transform.h"
#include "../shader/shader.h"
#include "mesh.h"
#include "model.h"
#include "meshrenderer.h"


namespace geeL{

	MeshRenderer::MeshRenderer(Transform& transform, CullingMode faceCulling)
		: transform(transform), model(nullptr), faceCulling(faceCulling), instanced(true) {}

	MeshRenderer::MeshRenderer(Transform& transform, Model& model, CullingMode faceCulling)
		: transform(transform), model(&model), faceCulling(faceCulling), instanced(false) {}


	void MeshRenderer::draw(bool shade) {

		switch (faceCulling) {
			case cullNone:
				glDisable(GL_CULL_FACE);
			case cullBack:
				glCullFace(GL_BACK);
		}

		//Draw model
		//But only if the mesh renderer is not instanced
		//In that case the models geometry will be drawn instanced
		//by the model drawer class
		if (!instanced && model != nullptr) {
			transformMeshes(*model);

			if (customMaterials.size() > 0)
				model->draw(customMaterials);
			else
				model->draw(shade);
		}

		switch (faceCulling) {
			case cullNone:
				glEnable(GL_CULL_FACE);
			case cullBack:
				glCullFace(GL_FRONT);
		}
	}

	void MeshRenderer::transformMeshes(Model& model) {
		//Load transform into vertex shaders
		for (vector<Mesh>::iterator it = model.meshesBegin(); it != model.meshesEnd(); it++) {
			Mesh& mesh = *it;
			Shader& shader = mesh.material.shader;

			shader.use();
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(transform.matrix));
		}
	}

}