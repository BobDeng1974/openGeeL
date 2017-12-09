#include "cameras/camera.h"
#include "animation/skeleton.h"
#include "materials/defaultmaterial.h"
#include "materials/material.h"
#include "transformation/transform.h"
#include "shader/sceneshader.h"
#include "mesh.h"
#include "model.h"
#include "instancedmesh.h"
#include "meshrenderer.h"

using namespace std;

namespace geeL{

	MeshRenderer::MeshRenderer(Transform& transform, 
		std::shared_ptr<Model> model,
		CullingMode faceCulling, 
		const std::string& name) 
			: SceneObject(transform, name)
			, modelData(model)
			, faceCulling(faceCulling)
			, mask(RenderMask::None) {}


	MeshRenderer::~MeshRenderer() {
		for (auto it(meshes.begin()); it != meshes.end(); it++)
			delete *it;
	}


	void MeshRenderer::setRenderMask(RenderMask mask) {
		this->mask = mask;
	}

	void MeshRenderer::setRenderMask(RenderMask mask, const MeshInstance& mesh) {
		MaterialMapping* mapping = getMapping(mesh);

		if (mapping != nullptr)
			mapping->mask = mask;
	}

	void MeshRenderer::drawMask(const MaterialMapping& mapping) const {
		const SceneShader& shader = mapping.material.getShader();
		RenderMask shaderMask = Masking::getShadingMask(shader.getMethod());
		RenderMask activeMask = shaderMask | mapping.mask | mask;

		if ((activeMask & RenderMask::Empty) != RenderMask::Empty)
			Masking::drawMask(activeMask);
		else
			Masking::passthrough();
	}

	void MeshRenderer::addMaterialChangeListener(std::function<void(MeshRenderer&, Material, Material)> listener) {
		materialListeners.push_back(listener);
	}

	void MeshRenderer::setAutomaticFiltering(bool value) {
		autoFilter = value;
	}

	bool MeshRenderer::isVisible(const Camera& camera) const {
		return !autoFilter || !camera.isBehind(transform.getPosition());
	}

	void MeshRenderer::draw(SceneShader& shader) const {
		CullingGuard culling(faceCulling);
		StencilGuard stencil;

		auto it = materials.find(&shader);
		if (it != materials.end()) {
			shader.bindMatrices(transform);

			const std::list<MaterialMapping>& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MaterialMapping& container = *et;

				//Draw individual material
				const Material& mat = container.material;
				mat.bind();

				drawMask(container);

				//Draw mesh
				const MeshInstance& mesh = container.mesh;
				mesh.draw(shader);
			}
		}
	}

	void MeshRenderer::drawExclusive(SceneShader& shader) const {
		shader.bindMatrices(transform);

		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<MaterialMapping>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MaterialMapping& mapping = *et;

				//Draw individual material
				const Material& mat = mapping.material;
				const MaterialContainer& container = mat.getMaterialContainer();
				container.bind(shader);

				//Draw mesh
				const MeshInstance& mesh = mapping.mesh;
				mesh.draw(shader);
			}
		}
	}

	void MeshRenderer::drawGeometry(const RenderShader& shader) const {
		transform.bind(shader, "model");

		iterateMeshes([&shader](const MeshInstance& mesh) {
			mesh.draw(shader);
		});
	}

	void MeshRenderer::changeMaterial(Material& material, const MeshInstance& mesh) {
		//Remove old element from materials since new material probably uses a different shader
		std::list<MaterialMapping>* elements;
		MaterialMapping* toRemove = nullptr;
		for (auto it = materials.begin(); it != materials.end(); it++) {
			elements = &it->second;
			for (auto et = elements->begin(); et != elements->end(); et++) {
				MaterialMapping& container = *et;

				if (container.mesh == mesh) {
					toRemove = &container;
					break;
				}
			}

			if (toRemove != nullptr) {
				Material oldMaterial = toRemove->material;
				elements->remove(*toRemove);

				//Add moved 'copy' of new material
				SceneShader& shader = material.getShader();
				materials[&shader].push_back(MaterialMapping(toRemove->mesh, material, toRemove->mask));

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

	void MeshRenderer::changeMaterial(SceneShader& shader, const MeshInstance& mesh) {
		//Find mesh in this mesh renderer
		for (auto it = materials.begin(); it != materials.end(); it++) {
			std::list<MaterialMapping>* elements = &it->second;

			for (auto et = elements->begin(); et != elements->end(); et++) {
				MaterialMapping& container = *et;

				if (container.mesh == mesh) {
					//Create new material with given shader and old container
					Material material(container.material, shader);
					changeMaterial(material, mesh);

					return;
				}
			}
		}
	}

	void MeshRenderer::iterate(std::function<void(const MeshInstance&, const Material&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<MaterialMapping>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++) {
				const Material& mat = et->material;
				const MeshInstance& mesh = et->mesh;

				function(mesh, mat);
			}
		}
	}

	void MeshRenderer::iterateMeshes(std::function<void(const MeshInstance&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<MaterialMapping>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++)
				function(et->mesh);
		}
	}

	void MeshRenderer::iterateMeshesSafe(std::function<void(const MeshInstance&)> function) const {
		std::list<const MeshInstance*> tempMeshes;

		for (auto it(materials.begin()); it != materials.end(); it++) {
			const std::list<MaterialMapping>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++)
				tempMeshes.push_back(&et->mesh);
		}

		for (auto it(tempMeshes.begin()); it != tempMeshes.end(); it++)
			function(**it);

	}

	void MeshRenderer::iterateMaterials(std::function<void(MaterialContainer&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<MaterialMapping>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++) {
				const Material& mat = et->material;
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

	const MeshInstance* MeshRenderer::getMesh(const std::string& name) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<MaterialMapping>* elements = &it->second;
			for (auto et = elements->begin(); et != elements->end(); et++) {
				const MeshInstance& mesh = et->mesh;

				if (mesh.getName() == name)
					return &mesh;
			}
		}

		return nullptr;
	}

	bool MeshRenderer::containsShader(SceneShader& shader) const {
		return materials.find(&shader) != materials.end();
	}

	

	MaterialMapping* MeshRenderer::getMapping(const MeshInstance& mesh) {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			std::list<MaterialMapping>* elements = &it->second;
			for (auto et = elements->begin(); et != elements->end(); et++) {
				MaterialMapping& container = *et;

				if (container.mesh == mesh)
					return &container;
			}
		}

		return nullptr;
	}



	StaticMeshRenderer::StaticMeshRenderer(Transform& transform, 
		SceneShader& shader, 
		std::shared_ptr<StaticModel> model,
		CullingMode faceCulling, 
		const std::string& name)
			: MeshRenderer(transform, model, faceCulling, name) {
	
		initMaterials(shader, *model);
	}

	StaticMeshRenderer::StaticMeshRenderer(Transform& transform, 
		SceneShader& shader, 
		std::shared_ptr<StaticModel> model,
		std::list<const StaticMesh*>& meshes,
		CullingMode faceCulling, 
		const std::string & name)
			: MeshRenderer(transform, model, faceCulling, name) {

		initMaterials(shader, meshes);
	}


	RenderMode StaticMeshRenderer::getRenderMode() const {
		return RenderMode::Static;
	}

	void StaticMeshRenderer::initMaterials(SceneShader& shader, StaticModel& model) {
		model.iterateMeshesGeneric([&](const StaticMesh& mesh) {
			meshes.push_back(new StaticMeshInstance(mesh));
		});

		for (auto it(meshes.begin()); it != meshes.end(); it++) {
			MeshInstance& mesh = **it;

			MaterialContainer& container = mesh.getMaterialContainer();
			Material& material = Material(shader, container);

			materials[&shader].push_back(MaterialMapping(mesh, material));
		}
	}

	void StaticMeshRenderer::initMaterials(SceneShader& shader, std::list<const StaticMesh*>& newMeshes) {
		for (auto it(newMeshes.begin()); it != newMeshes.end(); it++) {
			const StaticMesh& mesh = **it;
			meshes.push_back(new StaticMeshInstance(mesh));
		}

		for (auto it(meshes.begin()); it != meshes.end(); it++) {
			MeshInstance& mesh = **it;

			MaterialContainer& container = mesh.getMaterialContainer();
			Material& material = Material(shader, container);

			materials[&shader].push_back(MaterialMapping(mesh, material));
		}
	}



	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform,
		SceneShader& shader,
		std::shared_ptr<SkinnedModel> model,
		CullingMode faceCulling,
		const std::string& name)
			: MeshRenderer(transform, model, faceCulling, name)
			, skinnedModel(*model)
			, skeleton(new Skeleton(model->getSkeleton())) {

		initMaterials(shader);

		//Add transform of skeleton to the transform of the actual model
		//and therefore into the whole scene structure. Thus, updating 
		//transform information will be taken care off
		skeleton->setParent(transform);
	}

	SkinnedMeshRenderer::~SkinnedMeshRenderer() {
		if (skeleton != nullptr)
			delete skeleton;
	}


	Skeleton& SkinnedMeshRenderer::getSkeleton() {
		return *skeleton;
	}

	SkinnedModel& SkinnedMeshRenderer::getSkinnedModel() {
		return skinnedModel;
	}

	RenderMode SkinnedMeshRenderer::getRenderMode() const {
		return RenderMode::Skinned;
	}

	void SkinnedMeshRenderer::initMaterials(SceneShader& shader) {
		skinnedModel.iterateMeshesGeneric([&](const SkinnedMesh& mesh) {
			meshes.push_back(new SkinnedMeshInstance(mesh, *skeleton));
		});

		for (auto it(meshes.begin()); it != meshes.end(); it++) {
			MeshInstance& mesh = **it;

			MaterialContainer& container = mesh.getMaterialContainer();
			Material& material = Material(shader, container);

			materials[&shader].push_back(MaterialMapping(mesh, material));
		}
	}

}