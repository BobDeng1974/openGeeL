#include "cameras/camera.h"
#include "animation/skeleton.h"
#include "materials/defaultmaterial.h"
#include "materials/material.h"
#include "transformation/transform.h"
#include "shader/sceneshader.h"
#include "mesh.h"
#include "model.h"
#include "meshidgenerator.h"
#include "instancedmesh.h"
#include "meshrenderer.h"

using namespace std;

namespace geeL{

	MeshRenderer::MeshRenderer(Transform& transform, 
		MemoryObject<Model> model,
		size_t meshCount,
		CullingMode faceCulling, 
		const std::string& name) 
			: SceneObject(transform, name)
			, modelData(model)
			, faceCulling(faceCulling)
			, mask(RenderMask::None)
			, id(MeshRendererIDGenerator::generateID(*this, meshCount))
			, aabb(TransformableBoundingBox(modelData->getBoundingBox(), transform)) {}

	MeshRenderer::~MeshRenderer() {
		for (auto it(deleteListeners.begin()); it != deleteListeners.end(); it++)
			(*it)(*this);

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

	void MeshRenderer::addDeleteListener(std::function<void(const MeshRenderer&)> listener) {
		deleteListeners.push_back(listener);
	}

	bool MeshRenderer::isVisible(const SceneCamera& camera) const {
		const ViewFrustum& frustum = camera.getFrustum();
		IntersectionType t = aabb.intersect(frustum);

		return t != IntersectionType::Outside;
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
				const MeshInstance& mesh = container.mesh;

				//Draw individual material
				const Material& mat = container.material;
				shader.bind<unsigned int>("id", mesh.getID());
				mat.bind();
				
				drawMask(container);
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

				//Remove shader bucket if all associated materials have been removed
				if (elements->size() == 0)
					materials.erase(it);

				//Inform material change listeners
				for (auto it(materialListeners.begin()); it != materialListeners.end(); it++)
					(*it)(*this, oldMaterial, material);

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
		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			const MeshInstance& mesh = **it;
			function(mesh);
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

	size_t MeshRenderer::getMeshCount() const {
		return meshes.size();
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

	void MeshRenderer::addMesh(MeshInstance* mesh) {
		mesh->setID(MeshRendererIDGenerator::generateMeshID(*this));
		meshes.push_back(mesh);
	}

	unsigned short MeshRenderer::getID() const {
		return id;
	}



	StaticMeshRenderer::StaticMeshRenderer(Transform& transform, 
		SceneShader& shader, 
		MemoryObject<StaticModel> model,
		CullingMode faceCulling, 
		const std::string& name)
			: MeshRenderer(transform, model, model->meshCount(), faceCulling, name) {
	
		initialize(shader, *model);
	}

	StaticMeshRenderer::StaticMeshRenderer(Transform& transform, 
		SceneShader& shader, 
		MemoryObject<StaticModel> model,
		std::list<const StaticMesh*>& meshes,
		CullingMode faceCulling, 
		const std::string& name)
			: MeshRenderer(transform, model, meshes.size(), faceCulling, name) {

		initialize(shader, meshes);
	}


	RenderMode StaticMeshRenderer::getRenderMode() const {
		return RenderMode::Static;
	}

	void StaticMeshRenderer::initialize(SceneShader& shader, StaticModel& model) {
		model.iterateMeshesGeneric([&](const StaticMesh& mesh) {
			addMesh(new StaticMeshInstance(mesh, transform));
		});

		iterateMeshes([&](const MeshInstance& mesh) {
			MaterialContainer& container = mesh.getMaterialContainer();
			Material& material = Material(shader, container);

			materials[&shader].push_back(MaterialMapping(mesh, material));
		});
	}

	void StaticMeshRenderer::initialize(SceneShader& shader, std::list<const StaticMesh*>& newMeshes) {
		for (auto it(newMeshes.begin()); it != newMeshes.end(); it++) {
			const StaticMesh& mesh = **it;
			addMesh(new StaticMeshInstance(mesh, transform));
		}

		iterateMeshes([&](const MeshInstance& mesh) {
			MaterialContainer& container = mesh.getMaterialContainer();
			Material& material = Material(shader, container);

			materials[&shader].push_back(MaterialMapping(mesh, material));
		});
	}



	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform,
		SceneShader& shader,
		MemoryObject<SkinnedModel> model,
		CullingMode faceCulling,
		const std::string& name)
			: MeshRenderer(transform, model, model->meshCount(), faceCulling, name)
			, skinnedModel(*model)
			, skeleton(new Skeleton(model->getSkeleton())) {

		initialize(shader);

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

	void SkinnedMeshRenderer::initialize(SceneShader& shader) {
		skinnedModel.iterateMeshesGeneric([&](const SkinnedMesh& mesh) {
			addMesh(new SkinnedMeshInstance(mesh, transform, *skeleton));
		});

		iterateMeshes([&](const MeshInstance& mesh) {
			MaterialContainer& container = mesh.getMaterialContainer();
			Material& material = Material(shader, container);

			materials[&shader].push_back(MaterialMapping(mesh, material));
		});
	}

}