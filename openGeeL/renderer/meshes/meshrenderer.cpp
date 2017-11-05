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


	void MeshRenderer::draw(SceneShader& shader) const {
		CullingGuard culling(faceCulling);
		StencilGuard stencil;

		auto it = materials.find(&shader);
		if (it != materials.end()) {
			shader.setModelMatrix(transform.getMatrix());
			shader.bindMatrices();

			if (shader.getSpace() == ShaderTransformSpace::World)
				shader.bind<glm::mat4>("model", transform.getMatrix());

			const std::list<MaterialMapping>& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MaterialMapping& container = *et;

				//Draw individual material
				const Material& mat = container.material;
				mat.bind();

				Masking::drawMask(getMask(container));

				//Draw mesh
				const Mesh& mesh = *container.mesh;
				mesh.draw();
			}
		}
	}

	void MeshRenderer::drawExclusive(SceneShader& shader) const {
		shader.setModelMatrix(transform.getMatrix());
		shader.bindMatrices();

		if (shader.getSpace() == ShaderTransformSpace::World)
			shader.bind<glm::mat4>("model", transform.getMatrix());

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

	void MeshRenderer::setRenderMask(RenderMask mask) {
		this->mask = mask;
	}

	void MeshRenderer::setRenderMask(RenderMask mask, const Mesh& mesh) {
		MaterialMapping* mapping = getMapping(mesh);

		if (mapping != nullptr)
			mapping->mask = mask;
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

	RenderMask MeshRenderer::getMask(const MaterialMapping& mapping) const {
		//Mask of mesh renderer overrides mask of single mesh.
		//Therefore, it is only returned if no base mask exists
		return (mask == RenderMask::None) ? mapping.mask : mask;
	}

	MeshRenderer::MaterialMapping * MeshRenderer::getMapping(const Mesh& mesh) {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			std::list<MaterialMapping>* elements = &it->second;
			for (auto et = elements->begin(); et != elements->end(); et++) {
				MaterialMapping& container = *et;

				if (container.mesh == &mesh)
					return &container;
			}
		}

		return nullptr;
	}

	


	void MeshRenderer::iterate(std::function<void(const Mesh&, const Material&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<MaterialMapping>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++) {
				const Material& mat = (*et).material;
				const Mesh& mesh = *(*et).mesh;

				function(mesh, mat);
			}
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

	const Mesh* MeshRenderer::getMesh(const std::string& name) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<MaterialMapping>* elements = &it->second;
			for (auto et = elements->begin(); et != elements->end(); et++) {
				const Mesh& mesh = *et->mesh;

				if (mesh.getName() == name)
					return &mesh;
			}
		}

		return nullptr;
	}

	RenderMode MeshRenderer::getRenderMode() const {
		return RenderMode::Static;
	}

	bool MeshRenderer::containsShader(SceneShader& shader) const {
		return materials.find(&shader) != materials.end();
	}
}