#include "animation/skeleton.h"
#include "cameras/camera.h"
#include "shader/sceneshader.h"
#include "mesh.h"
#include "meshidgenerator.h"
#include "model.h"
#include "singlemeshrenderer.h"

namespace geeL {

	SingleMeshRenderer::SingleMeshRenderer(Transform& transform, const Mesh& mesh, 
		SceneShader& shader, MemoryObject<Model> modelData,
		CullingMode faceCulling, const std::string & name)
			: SceneObject(transform, name)
			, faceCulling(faceCulling)
			, mask(RenderMask::None)
			, id(MeshRendererIDGenerator::generateID(*this, 1))
			, modelData(modelData)
			, material(Material(shader, mesh.getMaterialContainer()))
			, aabb(TransformableBoundingBox(mesh.getBoundingBox(), transform)) {}

	SingleMeshRenderer::~SingleMeshRenderer() {
		for (auto it(deleteListeners.begin()); it != deleteListeners.end(); it++)
			(*it)(*this);
	}


	void SingleMeshRenderer::draw(SceneShader& s) const {
		CullingGuard culling(faceCulling);
		StencilGuard stencil;

		const SceneShader& shader = material.getShader();
		shader.bindMatrices(transform);

		shader.bind<unsigned int>("id", getID());
		material.bind();

		drawMask();
		drawMesh(shader);
	}

	void SingleMeshRenderer::drawExclusive(SceneShader& shader) const {
		CullingGuard culling(faceCulling);
		StencilGuard stencil;

		const MaterialContainer& container = material.getMaterialContainer();
		shader.bindMatrices(transform);

		container.bind(shader);
		drawMesh(shader);
	}

	void SingleMeshRenderer::drawGeometry(const RenderShader& shader) const {
		transform.bind(shader, "model");
		drawMesh(shader);
	}

	void SingleMeshRenderer::setRenderMask(RenderMask mask) {
		this->mask = mask;
	}

	Material& SingleMeshRenderer::getMaterial() {
		return material;
	}

	const Material& SingleMeshRenderer::getMaterial() const {
		return material;
	}

	void SingleMeshRenderer::setMaterial(Material& material) {
		Material oldMaterial(this->material);
		this->material = material;

		for (auto it(materialListeners.begin()); it != materialListeners.end(); it++)
			(*it)(*this, oldMaterial, material);
	}

	SceneShader& SingleMeshRenderer::getShader() {
		return material.getShader();
	}

	const SceneShader& SingleMeshRenderer::getShader() const {
		return material.getShader();
	}

	void SingleMeshRenderer::setShader(SceneShader& shader) {
		Material oldMaterial(this->material);
		material.setShader(shader);

		for (auto it(materialListeners.begin()); it != materialListeners.end(); it++)
			(*it)(*this, oldMaterial, material);
	}

	void SingleMeshRenderer::addMaterialChangeListener(std::function<void(SingleMeshRenderer&, Material, Material)> listener) {
		materialListeners.push_back(listener);
	}

	void SingleMeshRenderer::addDeleteListener(std::function<void(const SingleMeshRenderer&)> listener) {
		deleteListeners.push_back(listener);
	}

	const AABoundingBox& SingleMeshRenderer::getBoundingBox() const {
		return aabb;
	}

	bool SingleMeshRenderer::isVisible(const Camera& camera) const {
		return isVisible(camera.getFrustum());
	}

	bool SingleMeshRenderer::isVisible(const ViewFrustum& view) const {
		return aabb.intersect(view) != IntersectionType::Outside;
	}

	unsigned short SingleMeshRenderer::getID() const {
		return id;
	}

	void SingleMeshRenderer::drawMask() const {
		const SceneShader& shader = material.getShader();
		RenderMask shaderMask = Masking::getShadingMask(shader.getMethod());
		RenderMask activeMask = shaderMask | mask;

		if ((activeMask & RenderMask::Empty) != RenderMask::Empty)
			Masking::drawMask(activeMask);
		else
			Masking::passthrough();

	}



	SingleStaticMeshRenderer::SingleStaticMeshRenderer(Transform& transform, 
		const StaticMesh& mesh, SceneShader& shader, MemoryObject<StaticModel> model,
		CullingMode faceCulling, const std::string& name)
			: SingleMeshRenderer(transform, mesh, shader, model, faceCulling, name)
			, mesh(mesh) {}


	RenderMode SingleStaticMeshRenderer::getRenderMode() const {
		return RenderMode::Static;
	}

	const Mesh& SingleStaticMeshRenderer::getMesh() const {
		return mesh;
	}

	void SingleStaticMeshRenderer::drawMesh(const Shader& shader) const {
		mesh.draw(shader);
	}



	SingleSkinnedMeshRenderer::SingleSkinnedMeshRenderer(Transform& transform, const SkinnedMesh& mesh, 
		SceneShader& shader, MemoryObject<SkinnedModel> model, 
		CullingMode faceCulling, const std::string& name)
			: SingleMeshRenderer(transform, mesh, shader, model, faceCulling, name)
			, mesh(mesh)
			, animationContainer(*model) 
			, skeleton(new Skeleton(model->getSkeleton())) {
	
		//Add transform of skeleton to the transform of the actual model
		//and therefore into the whole scene structure. Thus, updating 
		//transform information will be taken care off
		skeleton->setParent(transform);
	}

	SingleSkinnedMeshRenderer::SingleSkinnedMeshRenderer(Transform& transform, const SkinnedMesh& mesh, 
		SceneShader& shader, MemoryObject<SkinnedModel> model, std::shared_ptr<Skeleton> skeleton, 
		CullingMode faceCulling, const std::string & name)
			: SingleMeshRenderer(transform, mesh, shader, model, faceCulling, name)
			, mesh(mesh)
			, animationContainer(*model)
			, skeleton(skeleton) {}


	std::shared_ptr<Skeleton> SingleSkinnedMeshRenderer::shareSkeleton() {
		return skeleton;
	}

	Skeleton& SingleSkinnedMeshRenderer::getSkeleton() {
		return *skeleton;
	}

	AnimationContainer& SingleSkinnedMeshRenderer::getAnimationContainer() {
		return animationContainer;
	}

	RenderMode SingleSkinnedMeshRenderer::getRenderMode() const {
		return RenderMode::Skinned;
	}

	const Mesh& SingleSkinnedMeshRenderer::getMesh() const {
		return mesh;
	}

	void SingleSkinnedMeshRenderer::drawMesh(const Shader& shader) const {
		mesh.updateBones(shader, *skeleton);
		mesh.draw(shader);
	}



	MeshRendererGroup::MeshRendererGroup(Transform& transform, SceneShader& shader, 
		std::list<SingleMeshRenderer*> renderers, 
		CullingMode faceCulling, const std::string & name)
			: SingleMeshRenderer(transform, renderers.front()->getMesh(), shader, nullptr, faceCulling, name) 
			, renderers(std::move(renderers)) {

		assert(renderers.size() > 0 && "At least one mesh renderers has to be attached to this group");

		aabb.reset();
		mode = renderers.front()->getRenderMode();
		for (auto it(renderers.begin()); it != renderers.end(); it++) {
			SingleMeshRenderer& r = **it;

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
			SingleMeshRenderer& r = **it;
			Material& m = r.getMaterial();

			m.bind();
			r.drawMesh(shader);
		}
	}

	void MeshRendererGroup::drawExclusive(SceneShader& shader) const {
		CullingGuard culling(faceCulling);
		StencilGuard stencil;

		shader.bindMatrices(transform);
		for (auto it(renderers.begin()); it != renderers.end(); it++) {
			SingleMeshRenderer& r = **it;
			MaterialContainer& m = r.getMaterial().getMaterialContainer();

			m.bind(shader);
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
			SingleMeshRenderer& r = **it;
			r.drawMesh(shader);
		}
	}

}