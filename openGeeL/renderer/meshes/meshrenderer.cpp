#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "cameras/camera.h"
#include "animation/skeleton.h"
#include "materials/defaultmaterial.h"
#include "materials/material.h"
#include "transformation/transform.h"
#include "shader/sceneshader.h"
#include "mesh.h"
#include "model.h"
#include "meshrenderer.h"

using namespace std;

namespace geeL{

	MeshRenderer::MeshRenderer(Transform& transform, 
		CullingMode faceCulling, 
		const std::string & name) 
			: SceneObject(transform, name)
			, faceCulling(faceCulling)
			, mask(RenderMask::None) {}


	void MeshRenderer::setRenderMask(RenderMask mask) {
		this->mask = mask;
	}

	void MeshRenderer::setRenderMask(RenderMask mask, const Mesh& mesh) {
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



	StaticMeshRenderer::StaticMeshRenderer(Transform& transform, 
		SceneShader& shader, 
		StaticModel& model, 
		CullingMode faceCulling, 
		const std::string& name)
			: GenericMeshRenderer<StaticMesh>(transform, shader, model, faceCulling, name) {}

	RenderMode StaticMeshRenderer::getRenderMode() const {
		return RenderMode::Static;
	}



	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform,
		SceneShader& shader,
		SkinnedModel& model,
		CullingMode faceCulling,
		const std::string& name)
			: GenericMeshRenderer<SkinnedMesh>(transform, shader, model, faceCulling, name)
			, skinnedModel(model)
			, skeleton(new Skeleton(model.getSkeleton())) {

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

}