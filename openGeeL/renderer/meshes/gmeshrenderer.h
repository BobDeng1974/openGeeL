#ifndef GENERICMESHRENDERER_H
#define GENERICMESHRENDERER_H

#include <map>
#include "cameras/camera.h"
#include "materials/material.h"
#include "transformation/transform.h"
#include "shader/sceneshader.h"
#include "mesh.h"
#include "model.h"
#include "ameshrenderer.h"

namespace geeL {

	template<typename MeshType>
	class GenericMeshRenderer : public MeshRenderer {

	public:
		GenericMeshRenderer(Transform& transform,
			SceneShader& shader,
			GenericModel<MeshType>& model,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "MeshRenderer");

		virtual ~GenericMeshRenderer() {}


		virtual void draw(SceneShader& shader) const;
		virtual void drawExclusive(SceneShader& shader) const;
		virtual void drawGeometry(const RenderShader& shader) const;

		virtual void changeMaterial(Material& material, const Mesh& mesh);
		virtual void changeMaterial(SceneShader& material, const Mesh& mesh);

		virtual void iterate(std::function<void(const Mesh&, const Material&)> function) const;
		virtual void iterateMeshes(std::function<void(const Mesh&)> function) const;
		virtual void iterateMaterials(std::function<void(MaterialContainer&)> function) const;
		virtual void iterateShaders(std::function<void(const SceneShader&)> function) const;
		virtual void iterateShaders(std::function<void(SceneShader&)> function);

		virtual const Model& getModel() const;
		virtual const Mesh* getMesh(const std::string& name) const;
		virtual RenderMode getRenderMode() const = 0;

		virtual bool containsShader(SceneShader& shader) const;

	protected:
		GenericModel<MeshType>* model;
		std::map<SceneShader*, std::list<GenericMaterialMapping<MeshType>>> materials;

		//Init materials with data from the meshes material containers
		void initMaterials(SceneShader& shader);

		virtual MaterialMapping* getMapping(const Mesh& mesh);

	};



	template<typename MeshType>
	inline GenericMeshRenderer<MeshType>::GenericMeshRenderer(Transform& transform,
		SceneShader& shader,
		GenericModel<MeshType>& model,
		CullingMode faceCulling,
		const std::string& name)
			: MeshRenderer(transform, faceCulling, name)
			, model(&model) {

		initMaterials(shader);
	}

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::draw(SceneShader& shader) const {
		CullingGuard culling(faceCulling);
		StencilGuard stencil;

		auto it = materials.find(&shader);
		if (it != materials.end()) {
			shader.bindMatrices(transform);

			const std::list<GenericMaterialMapping<MeshType>>& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MaterialMapping& container = *et;

				//Draw individual material
				const Material& mat = container.material;
				mat.bind();

				drawMask(container);

				//Draw mesh
				const Mesh& mesh = container.getMesh();
				mesh.draw(shader);
			}
		}
	}

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::drawExclusive(SceneShader& shader) const {
		shader.bindMatrices(transform);

		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<GenericMaterialMapping<MeshType>>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MaterialMapping& mapping = *et;

				//Draw individual material
				const Material& mat = mapping.material;
				const MaterialContainer& container = mat.getMaterialContainer();
				container.bind(shader);

				//Draw mesh
				const Mesh& mesh = mapping.getMesh();
				mesh.draw(shader);
			}
		}
	}

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::drawGeometry(const RenderShader& shader) const {
		transform.bind(shader, "model");

		model->iterateMeshes([&shader](const MeshType& mesh) {
			mesh.draw(shader);
		});
	}

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::changeMaterial(Material& material, const Mesh& mesh) {
		//Remove old element from materials since new material probably uses a different shader
		std::list<GenericMaterialMapping<MeshType>>* elements;
		GenericMaterialMapping<MeshType>* toRemove = nullptr;
		for (auto it = materials.begin(); it != materials.end(); it++) {
			elements = &it->second;
			for (auto et = elements->begin(); et != elements->end(); et++) {
				GenericMaterialMapping<MeshType>& container = *et;

				if (&container.getMesh() == &mesh) {
					toRemove = &container;
					break;
				}
			}

			if (toRemove != nullptr) {
				Material oldMaterial = toRemove->material;
				elements->remove(*toRemove);

				//Add moved 'copy' of new material
				SceneShader& shader = material.getShader();
				materials[&shader].push_back(GenericMaterialMapping<MeshType>(toRemove->mesh, material, toRemove->mask));

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

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::changeMaterial(SceneShader& shader, const Mesh& mesh) {
		//Find mesh in this mesh renderer
		for (auto it = materials.begin(); it != materials.end(); it++) {
			std::list<GenericMaterialMapping<MeshType>>* elements = &it->second;

			for (auto et = elements->begin(); et != elements->end(); et++) {
				MaterialMapping& container = *et;

				if (&container.getMesh() == &mesh) {
					//Create new material with given shader and old container
					Material material(container.material, shader);
					changeMaterial(material, mesh);

					return;
				}
			}
		}
	}

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::iterate(std::function<void(const Mesh&, const Material&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<GenericMaterialMapping<MeshType>>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++) {
				const Material& mat = et->material;
				const Mesh& mesh = et->getMesh();

				function(mesh, mat);
			}
		}
	}

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::iterateMeshes(std::function<void(const Mesh&)> function) const {
		model->iterateMeshes(function);
	}

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::iterateMaterials(std::function<void(MaterialContainer&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<GenericMaterialMapping<MeshType>>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++) {
				const Material& mat = et->material;
				function(mat.getMaterialContainer());
			}
		}
	}

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::iterateShaders(std::function<void(const SceneShader&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const SceneShader& shader = *it->first;
			function(shader);
		}
	}

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::iterateShaders(std::function<void(SceneShader&)> function) {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			SceneShader& shader = *it->first;
			function(shader);
		}
	}

	template<typename MeshType>
	inline const Model& GenericMeshRenderer<MeshType>::getModel() const {
		return *model;
	}

	template<typename MeshType>
	inline const Mesh* GenericMeshRenderer<MeshType>::getMesh(const std::string& name) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<GenericMaterialMapping<MeshType>>* elements = &it->second;
			for (auto et = elements->begin(); et != elements->end(); et++) {
				const Mesh& mesh = et->getMesh();

				if (mesh.getName() == name)
					return &mesh;
			}
		}

		return nullptr;
	}

	template<typename MeshType>
	inline bool GenericMeshRenderer<MeshType>::containsShader(SceneShader& shader) const {
		return materials.find(&shader) != materials.end();
	}

	template<typename MeshType>
	inline void GenericMeshRenderer<MeshType>::initMaterials(SceneShader& shader) {
		model->iterateMeshes([&](const MeshType& mesh) {
			MaterialContainer& container = mesh.getMaterialContainer();
			Material& material = Material(shader, container);

			materials[&shader].push_back(GenericMaterialMapping<MeshType>(mesh, material));
		});
	}

	template<typename MeshType>
	inline MaterialMapping* GenericMeshRenderer<MeshType>::getMapping(const Mesh& mesh) {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			std::list<GenericMaterialMapping<MeshType>>* elements = &it->second;
			for (auto et = elements->begin(); et != elements->end(); et++) {
				MaterialMapping& container = *et;

				if (&container.getMesh() == &mesh)
					return &container;
			}
		}

		return nullptr;
	}

}

#endif
