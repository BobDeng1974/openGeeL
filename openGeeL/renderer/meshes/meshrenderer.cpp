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

using namespace std;

namespace geeL{

	MeshRenderer::MeshRenderer(Transform& transform, SceneShader& shader, CullingMode faceCulling, std::string name)
		: SceneObject(transform, name), model(nullptr), faceCulling(faceCulling), instanced(true) {
	
		initMaterials(shader);
	}

	MeshRenderer::MeshRenderer(Transform& transform, SceneShader& shader, Model& model, CullingMode faceCulling, std::string name)
		: SceneObject(transform, name), model(&model), faceCulling(faceCulling), instanced(false) {
	
		initMaterials(shader);
	}

	MeshRenderer::~MeshRenderer() {
		for (auto it = deferredMaterials.begin(); it != deferredMaterials.end(); it++) {
			Material* mat = (*it).second;
			delete mat;
		}

		for (auto it = forwardMaterials.begin(); it != forwardMaterials.end(); it++) {
			Material* mat = (*it).second;
			delete mat;
		}
	}


	void MeshRenderer::draw(bool deferred) const {

		switch (faceCulling) {
			case cullNone:
				glDisable(GL_CULL_FACE);
			case cullBack:
				glCullFace(GL_BACK);
		}

		//Draw model
		if (!instanced && model != nullptr) {
			if (deferred) {
				transformMeshes(*model, deferredMaterials);
				model->draw(deferredMaterials);
			}
			else {
				transformMeshes(*model, forwardMaterials);
				model->draw(forwardMaterials);
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
			model->draw();
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
			Material* mat = materials[i];
			
			customizeMaterials(mat, i);
		}
	}

	void MeshRenderer::customizeMaterials(Material* material, unsigned int index) {
		if (index >= model->meshCount())
			return;

		if (material->rendersDeferred()) {
			deferredMaterials[index] = material;
			if (forwardMaterials.find(index) != forwardMaterials.end())
				forwardMaterials.erase(forwardMaterials.find(index));
		}
		else {
			forwardMaterials[index] = material;
			if (deferredMaterials.find(index) != deferredMaterials.end())
				deferredMaterials.erase(deferredMaterials.find(index));
		}
	}

	void MeshRenderer::initMaterials(SceneShader& shader) {

		//Load the default materials of the models meshes as materials of this mesh renderer
		int counter = 0;

		if (shader.deferred) {
			std::vector<MaterialContainer*> mats = model->getMaterials();
			for (auto it = mats.begin(); it != mats.end(); it++) {
				MaterialContainer& container = **it;
				deferredMaterials[counter] = new Material(shader, container);
				counter++;
			}
		}
		else {
			std::vector<MaterialContainer*> mats = model->getMaterials();
			for (auto it = mats.begin(); it != mats.end(); it++) {
				MaterialContainer& container = **it;
				forwardMaterials[counter] = new Material(shader, container);
				counter++;
			}
		}
	}

	void MeshRenderer::transformMeshes(Model& model, const Shader* shader) const {

		//Load transform into vertex shaders
		transformMeshes(model, deferredMaterials, shader);
		transformMeshes(model, forwardMaterials, shader);
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
			sha->setMat4("model", transform.getMatrix());
		}
	}

	bool MeshRenderer::containsForwardMaterials() const {
		return forwardMaterials.size() > 0;
	}

	bool MeshRenderer::containsDeferredMaterials() const {
		return deferredMaterials.size() > 0;
	}

	map<unsigned int, Material*>::iterator MeshRenderer::deferredMaterialsBegin() {
		return deferredMaterials.begin();
	}

	map<unsigned int, Material*>::iterator MeshRenderer::deferredMaterialsEnd() {
		return deferredMaterials.end();
	}

	map<unsigned int, Material*>::iterator MeshRenderer::forwardMaterialsBegin() {
		return forwardMaterials.begin();
	}

	map<unsigned int, Material*>::iterator MeshRenderer::forwardMaterialsEnd() {
		return forwardMaterials.end();
	}
}