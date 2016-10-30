#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "../materials/defaultmaterial.h"
#include "../materials/material.h"
#include "../transformation/transform.h"
#include "../shader/shader.h"
#include "mesh.h"
#include "model.h"
#include "meshrenderer.h"
#include <iostream>

using namespace std;

namespace geeL{

	MeshRenderer::MeshRenderer(Transform& transform, CullingMode faceCulling)
		: SceneObject(transform), model(nullptr), faceCulling(faceCulling), instanced(true) {
	
		initMaterials();
	}

	MeshRenderer::MeshRenderer(Transform& transform, Model& model, CullingMode faceCulling)
		: SceneObject(transform), model(&model), faceCulling(faceCulling), instanced(false) {
	
		initMaterials();
	}


	void MeshRenderer::draw(bool drawDefault) const {

		switch (faceCulling) {
			case cullNone:
				glDisable(GL_CULL_FACE);
			case cullBack:
				glCullFace(GL_BACK);
		}

		//Draw model
		if (!instanced && model != nullptr) {
			if (drawDefault) {
				transformMeshes(*model, defaultMaterials);
				model->draw(defaultMaterials);
			}
			else {
				transformMeshes(*model, customMaterials);
				model->draw(customMaterials);
			}
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
	

	void MeshRenderer::customizeMaterials(vector<Material*> materials) {
		int size = (materials.size() > model->meshCount()) 
			? model->meshCount()
			: materials.size();

		for (size_t i = 0; i < size; i++) {
			
			DefaultMaterial* mat = dynamic_cast<DefaultMaterial*>(materials[i]);
			if (mat != nullptr)
				defaultMaterials[i] = mat;
			else
				customMaterials[i] = materials[i];
		}
	}

	void MeshRenderer::initMaterials() {

		//Load the default materials of the models meshes as materials of this mesh renderer
		int counter = 0;
		for (vector<Mesh>::iterator it = model->meshesBegin(); it != model->meshesEnd(); it++) {
			defaultMaterials[counter] = &it->material;
			counter++;
		}
	}

	void MeshRenderer::transformMeshes(Model& model, const Shader* shader) const {

		//Load transform into vertex shaders
		transformMeshes(model, defaultMaterials, shader);
		transformMeshes(model, customMaterials, shader);
	}

	void MeshRenderer::transformMeshes(Model& model, 
		const map<unsigned int, Material*>& materials, const Shader* shader) const {

		for (map<unsigned int, Material*>::const_iterator it = materials.begin(); 
			it != materials.end(); it++) {

			unsigned int i = it->first;
			const Shader* sha;
			if (shader == nullptr) {
				Material* mat = it->second;
				sha = &mat->shader;
			}
			else
				sha = shader;

			sha->use();
			sha->setMat4("model", transform.matrix);
		}
	}

	bool MeshRenderer::hasIrregularMaterials() const {
		return customMaterials.size() > 0;
	}

}