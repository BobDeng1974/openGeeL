#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "materials/defaultmaterial.h"
#include "materials/material.h"
#include "transformation/transform.h"
#include "shader/sceneshader.h"
#include "mesh.h"
#include "model.h"
#include "meshrenderer.h"

using namespace std;

namespace geeL{

	MeshRenderer::MeshRenderer(Transform& transform, SceneShader& shader, Model& model, CullingMode faceCulling, const std::string& name)
		: SceneObject(transform, name), model(&model), faceCulling(faceCulling) {
	
		initMaterials(shader);
	}


	void MeshRenderer::draw() const {
		cullFaces();

		for (auto it = materials.begin(); it != materials.end(); it++) {
			//Activate and forward information to shader
			SceneShader& shader = *it->first;
			shader.setModelMatrix(transform.getMatrix());
			shader.bindMatrices();

			const std::list<MaterialMapping>& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MaterialMapping& container = *et;

				//Draw individual material
				const Material& mat = container.material;
				mat.bind();

				//Draw mesh
				const Mesh& mesh = *container.mesh;
				mesh.draw();
			}
		}

		uncullFaces();
	}

	void MeshRenderer::draw(SceneShader& shader) const {
		cullFaces();

		auto it = materials.find(&shader);
		if (it != materials.end()) {
			SceneShader& shader = *it->first;
			shader.setModelMatrix(transform.getMatrix());
			shader.bindMatrices();

			const std::list<MaterialMapping>& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MaterialMapping& container = *et;

				//Draw individual material
				const Material& mat = container.material;
				mat.bind();

				//Draw mesh
				const Mesh& mesh = *container.mesh;
				mesh.draw();
			}
		}

		uncullFaces();
	}

	void MeshRenderer::drawExclusive(SceneShader& shader) const {
		shader.setModelMatrix(transform.getMatrix());
		shader.bindMatrices();

		for (auto it = materials.begin(); it != materials.end(); it++) {
			const list<MaterialMapping>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MaterialMapping& mapping = *et;

				//Draw individual material
				const Material& mat = mapping.material;
				const MaterialContainer& container = mat.getMaterialContainer();
				container.bind(shader);

				//Draw mesh
				const Mesh& mesh = *mapping.mesh;
				mesh.draw();
			}
		}
	}

	void MeshRenderer::drawGeometry(const RenderShader& shader) const {
		shader.bind<glm::mat4>("model", transform.getMatrix());

		model->draw();
	}
	

	void MeshRenderer::changeMaterial(Material& material, const Mesh& mesh) {

		//Remove old element from materials since new material probably uses a different shader
		std::list<MaterialMapping>* elements;
		MaterialMapping* toRemove = nullptr;
		for (auto it = materials.begin(); it != materials.end(); it++) {
			elements = &it->second;
			for (auto et = elements->begin(); et != elements->end(); et++) {
				MaterialMapping& container = *et;

				if (container.mesh == &mesh) {
					toRemove = &container;
					break;
				}
			}

			if (toRemove != nullptr) {
				Material oldMaterial = toRemove->material;
				elements->remove(*toRemove);

				//Add moved 'copy' of new material
				SceneShader& shader = material.getShader();
				materials[&shader].push_back(MaterialMapping(mesh, material));

				//Inform material change listeners
				for (auto it(materialListeners.begin()); it != materialListeners.end(); it++)
					(*it)(*this, oldMaterial, material);

				//Remove shader bucket if all associated materials have been removed
				if (elements->size() == 0)
					materials.erase(it);

				return;
			}
		}
	}


	void MeshRenderer::changeMaterial(SceneShader& shader, const Mesh& mesh) {
		//Find mesh in this mesh renderer
		for (auto it = materials.begin(); it != materials.end(); it++) {
			std::list<MaterialMapping>* elements = &it->second;

			for (auto et = elements->begin(); et != elements->end(); et++) {
				MaterialMapping& container = *et;

				if (container.mesh == &mesh) {
					//Create new material with given shader and old container
					Material material(container.material, shader);
					changeMaterial(material, mesh);

					return;
				}
			}
		}
	}


	
	void MeshRenderer::initMaterials(SceneShader& shader) {
		model->iterateMeshes([&](const Mesh& mesh) {
			MaterialContainer& container = mesh.getMaterialContainer();
			Material& material = Material(shader, container);

			materials[&shader].push_back(MaterialMapping(mesh, material));
		});
	}


	void MeshRenderer::cullFaces() const {
		switch (faceCulling) {
			case CullingMode::cullNone:
				glDisable(GL_CULL_FACE);
				break;
			case CullingMode::cullBack:
				glCullFace(GL_BACK);
				break;
		}
	}

	void MeshRenderer::uncullFaces() const {
		switch (faceCulling) {
			case CullingMode::cullNone:
				glEnable(GL_CULL_FACE);
				break;
			case CullingMode::cullBack:
				glCullFace(GL_FRONT);
				break;
		}
	}


	void MeshRenderer::iterateMeshes(std::function<void(const Mesh&)> function) const {
		model->iterateMeshes(function);
	}

	void MeshRenderer::iterateMaterials(std::function<void(MaterialContainer&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<MaterialMapping>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++) {
				const Material& mat = (*et).material;
				function(mat.getMaterialContainer());
			}
		}
	}

	void MeshRenderer::iterateShaders(std::function<void(const SceneShader&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const SceneShader& shader = *it->first;
			function(shader);
		}
	}

	void MeshRenderer::iterateShaders(std::function<void(SceneShader&)> function) {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			SceneShader& shader = *it->first;
			function(shader);
		}
	}

	
	void MeshRenderer::addMaterialChangeListener(std::function<void(MeshRenderer&, Material, Material)> listener) {
		materialListeners.push_back(listener);
	}



	const Model& MeshRenderer::getModel() const {
		return *model;
	}

	RenderMode MeshRenderer::getRenderMode() const {
		return RenderMode::Static;
	}
}