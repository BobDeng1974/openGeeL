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


	void MeshRenderer::draw() const {

		switch (faceCulling) {
			case cullNone:
				glDisable(GL_CULL_FACE);
			case cullBack:
				glCullFace(GL_BACK);
		}

		//Draw model
		if (!instanced && model != nullptr) {
			transformMeshes(*model);
			model->draw(customMaterials);
		}

		switch (faceCulling) {
			case cullNone:
				glEnable(GL_CULL_FACE);
			case cullBack:
				glCullFace(GL_FRONT);
		}
	}

	void MeshRenderer::draw(const Shader& shader) const {

		switch (faceCulling) {
		case cullNone:
			glDisable(GL_CULL_FACE);
		case cullBack:
			glCullFace(GL_BACK);
		}

		//Draw model
		if (!instanced && model != nullptr) {
			transformMeshes(*model, &shader);
			model->draw(false);
		}

		switch (faceCulling) {
		case cullNone:
			glEnable(GL_CULL_FACE);
		case cullBack:
			glCullFace(GL_FRONT);
		}
	}

	void MeshRenderer::transformMeshes(Model& model, const Shader* shader) const {
		
		int counter = 0;
		//Load transform into vertex shaders
		for (vector<Mesh>::iterator it = model.meshesBegin(); it != model.meshesEnd(); it++) {
			Mesh& mesh = *it;

			const Shader* sha;
			if (shader == nullptr) {
				Material* mat = customMaterials[counter];
				sha = &mat->shader;
			}
			else
				sha = shader;

			sha->use();
			glUniformMatrix4fv(glGetUniformLocation(sha->program, "model"), 1, GL_FALSE, glm::value_ptr(transform.matrix));
			counter++;
		}
	}

	void MeshRenderer::customizeMaterials(vector<Material*> materials) {
		int size = (materials.size() > customMaterials.size()) 
			? customMaterials.size()
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