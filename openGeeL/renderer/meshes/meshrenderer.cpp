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
		: transform(transform), model(nullptr), faceCulling(faceCulling), instanced(true) {
	
		initMaterials();
	}

	MeshRenderer::MeshRenderer(Transform& transform, Model& model, CullingMode faceCulling)
		: transform(transform), model(&model), faceCulling(faceCulling), instanced(false) {
	
		initMaterials();
	}


	void MeshRenderer::draw(bool shade) const {

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

			if (shade)
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

	void MeshRenderer::transformMeshes(Model& model) const {
		//Load transform into vertex shaders
		for (vector<Mesh>::iterator it = model.meshesBegin(); it != model.meshesEnd(); it++) {
			Mesh& mesh = *it;
			Shader& shader = mesh.material.shader;

			shader.use();
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(transform.matrix));
		}
	}

	void MeshRenderer::customizeMaterials(vector<Material*> materials) {
		int size = (materials.size() > customMaterials.size()) ? customMaterials.size()
			: materials.size();

		for (size_t i = 0; i < size; i++)
			customMaterials[i] = materials[i];
	}

	void MeshRenderer::initMaterials() {

		//Load the default materials of the models meshes as materials of this mesh renderer
		for (vector<Mesh>::iterator it = model->meshesBegin(); it != model->meshesEnd(); it++) {
			customMaterials.push_back(&it->material);
		}
	}

	vector<Material*>::const_iterator MeshRenderer::materialsBegin() const {
		return customMaterials.begin();
	}

	vector<Material*>::const_iterator MeshRenderer::materialsEnd() const {
		return customMaterials.end();
	}

}