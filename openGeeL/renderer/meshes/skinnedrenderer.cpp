#include <iostream>
#include "shader/sceneshader.h"
#include "materials/material.h"
#include "transformation/transform.h"
#include "animation/skeleton.h"
#include "model.h"
#include "skinnedrenderer.h"

namespace geeL {

	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform, 
		SceneShader& shader, 
		SkinnedModel& model,
		CullingMode faceCulling, 
		const std::string& name)
			: MeshRenderer(transform, shader, model, faceCulling, name)
			, skinnedModel(&model)
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
		return *skinnedModel;
	}

	RenderMode SkinnedMeshRenderer::getRenderMode() const {
		return RenderMode::Skinned;
	}
}