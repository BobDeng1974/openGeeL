#include "animation/skeleton.h"
#include "cameras/camera.h"
#include "shader/sceneshader.h"
#include "mesh.h"
#include "meshidgenerator.h"
#include "model.h"
#include "meshrenderer.h"

namespace geeL {

	MeshRenderer::MeshRenderer(Transform& transform, const Mesh& mesh, 
		SceneShader& shader, MemoryObject<Model> modelData,
		CullingMode faceCulling, const std::string & name)
			: SceneObject(transform, name)
			, faceCulling(faceCulling)
			, mask(RenderMask::None)
			, id(MeshRendererIDGenerator::generateID(*this, 1))
			, modelData(modelData)
			, material(Material(shader, mesh.getMaterialContainer()))
			, aabb(TransformableBoundingBox(mesh.getBoundingBox(), transform)) {}

	MeshRenderer::~MeshRenderer() {
		for (auto it(deleteListeners.begin()); it != deleteListeners.end(); it++)
			(*it)(*this);
	}


	void MeshRenderer::draw() const {
		CullingGuard culling(faceCulling);
		StencilGuard stencil;

		const SceneShader& shader = material.getShader();
		shader.bindMatrices(transform);

		shader.bind<unsigned int>("id", getID());
		material.bind();
		material.loadMaps();

		drawMask();
		drawMesh(shader);
	}

	void MeshRenderer::drawExclusive(SceneShader& shader) const {
		CullingGuard culling(faceCulling);
		StencilGuard stencil;

		const MaterialContainer& container = material.getMaterialContainer();
		shader.bindMatrices(transform);

		container.bind(shader);
		shader.loadMaps();
		drawMesh(shader);
	}

	void MeshRenderer::drawGeometry(const RenderShader& shader) const {
		transform.bind(shader, "model");
		drawMesh(shader);
	}

	void MeshRenderer::setRenderMask(RenderMask mask) {
		this->mask = mask;
	}

	Material& MeshRenderer::getMaterial() {
		return material;
	}

	const Material& MeshRenderer::getMaterial() const {
		return material;
	}

	void MeshRenderer::setMaterial(Material& material) {
		Material oldMaterial(this->material);
		this->material = material;

		for (auto it(materialListeners.begin()); it != materialListeners.end(); it++)
			(*it)(*this, oldMaterial, material);
	}

	SceneShader& MeshRenderer::getShader() {
		return material.getShader();
	}

	const SceneShader& MeshRenderer::getShader() const {
		return material.getShader();
	}

	void MeshRenderer::setShader(SceneShader& shader) {
		Material oldMaterial(this->material);
		material.setShader(shader);

		for (auto it(materialListeners.begin()); it != materialListeners.end(); it++)
			(*it)(*this, oldMaterial, material);
	}

	void MeshRenderer::addMaterialChangeListener(std::function<void(MeshRenderer&, Material, Material)> listener) {
		materialListeners.push_back(listener);
	}

	void MeshRenderer::addDeleteListener(std::function<void(const MeshRenderer&)> listener) {
		deleteListeners.push_back(listener);
	}

	const AABoundingBox& MeshRenderer::getBoundingBox() const {
		return aabb;
	}

	bool MeshRenderer::isVisible(const Camera& camera) const {
		return isVisible(camera.getFrustum());
	}

	bool MeshRenderer::isVisible(const ViewFrustum& view) const {
		return aabb.intersect(view) != IntersectionType::Outside;
	}

	unsigned short MeshRenderer::getID() const {
		return id;
	}

	void MeshRenderer::drawMask() const {
		const SceneShader& shader = material.getShader();
		RenderMask shaderMask = Masking::getShadingMask(shader.getMethod());
		RenderMask activeMask = shaderMask | mask;

		if ((activeMask & RenderMask::Empty) != RenderMask::Empty)
			Masking::drawMask(activeMask);
		else
			Masking::passthrough();

	}



	StaticMeshRenderer::StaticMeshRenderer(Transform& transform, 
		const StaticMesh& mesh, SceneShader& shader, MemoryObject<StaticModel> model,
		CullingMode faceCulling, const std::string& name)
			: MeshRenderer(transform, mesh, shader, model, faceCulling, name)
			, mesh(mesh) {}


	RenderMode StaticMeshRenderer::getRenderMode() const {
		return RenderMode::Static;
	}

	const Mesh& StaticMeshRenderer::getMesh() const {
		return mesh;
	}

	void StaticMeshRenderer::drawMesh(const Shader& shader) const {
		mesh.draw(shader);
	}



	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform, const SkinnedMesh& mesh, 
		SceneShader& shader, MemoryObject<SkinnedModel> model, 
		CullingMode faceCulling, const std::string& name)
			: MeshRenderer(transform, mesh, shader, model, faceCulling, name)
			, mesh(mesh)
			, animationContainer(*model) 
			, skeleton(new Skeleton(model->getSkeleton())) {
	
		//Add transform of skeleton to the transform of the actual model
		//and therefore into the whole scene structure. Thus, updating 
		//transform information will be taken care off
		skeleton->setParent(transform);
	}

	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform, const SkinnedMesh& mesh, 
		SceneShader& shader, MemoryObject<SkinnedModel> model, std::shared_ptr<Skeleton> skeleton, 
		CullingMode faceCulling, const std::string & name)
			: MeshRenderer(transform, mesh, shader, model, faceCulling, name)
			, mesh(mesh)
			, animationContainer(*model)
			, skeleton(skeleton) {}


	std::shared_ptr<Skeleton> SkinnedMeshRenderer::shareSkeleton() {
		return skeleton;
	}

	Skeleton& SkinnedMeshRenderer::getSkeleton() {
		return *skeleton;
	}

	AnimationContainer& SkinnedMeshRenderer::getAnimationContainer() {
		return animationContainer;
	}

	RenderMode SkinnedMeshRenderer::getRenderMode() const {
		return RenderMode::Skinned;
	}

	const Mesh& SkinnedMeshRenderer::getMesh() const {
		return mesh;
	}

	void SkinnedMeshRenderer::drawMesh(const Shader& shader) const {
		mesh.updateBones(shader, *skeleton);
		mesh.draw(shader);
	}



	MeshRendererGroup::MeshRendererGroup(Transform& transform, SceneShader& shader, 
		std::list<MeshRenderer*> renderers, 
		CullingMode faceCulling, const std::string & name)
			: MeshRenderer(transform, renderers.front()->getMesh(), shader, nullptr, faceCulling, name) 
			, renderers(std::move(renderers)) {

		assert(renderers.size() > 0 && "At least one mesh renderers has to be attached to this group");

		aabb.reset();
		mode = renderers.front()->getRenderMode();
		for (auto it(renderers.begin()); it != renderers.end(); it++) {
			MeshRenderer& r = **it;

			aabb.extend(r.getBoundingBox());

			if (mode != r.getRenderMode())
				throw "All given mesh renderers should have the same render mode\n";
		}
	}

	void MeshRendererGroup::draw(SceneShader & shader) const {
		CullingGuard culling(faceCulling);
		StencilGuard stencil;

		shader.bindMatrices(transform);
		shader.bind<unsigned int>("id", getID());

		drawMask();
		for (auto it(renderers.begin()); it != renderers.end(); it++) {
			MeshRenderer& r = **it;
			Material& m = r.getMaterial();

			m.bind();
			m.loadMaps();
			r.drawMesh(shader);
		}
	}

	void MeshRendererGroup::drawExclusive(SceneShader& shader) const {
		CullingGuard culling(faceCulling);
		StencilGuard stencil;

		shader.bindMatrices(transform);
		for (auto it(renderers.begin()); it != renderers.end(); it++) {
			MeshRenderer& r = **it;
			MaterialContainer& m = r.getMaterial().getMaterialContainer();

			m.bind(shader);
			shader.loadMaps();
			r.drawMesh(shader);
		}
	}

	RenderMode MeshRendererGroup::getRenderMode() const {
		return mode;
	}

	const Mesh& MeshRendererGroup::getMesh() const {
		return renderers.front()->getMesh();
	}

	void MeshRendererGroup::drawMesh(const Shader & shader) const {
		for (auto it(renderers.begin()); it != renderers.end(); it++) {
			MeshRenderer& r = **it;
			r.drawMesh(shader);
		}
	}

}