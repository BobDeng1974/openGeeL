#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "gmeshrenderer.h"

namespace geeL {

	//Represents a drawn model in a render scene. Independent from actual model
	class StaticMeshRenderer : public GenericMeshRenderer<StaticMesh> {

	public:
		//Constructor for mesh renderer with an unique assigned model
		StaticMeshRenderer(Transform& transform, 
			SceneShader& shader, 
			StaticModel& model,
			CullingMode faceCulling = CullingMode::cullFront, 
			const std::string& name = "MeshRenderer");

		virtual ~StaticMeshRenderer() {}

		virtual RenderMode getRenderMode() const;

	};


	//Special mesh renderer that is intended for use with animated/skinned models
	class SkinnedMeshRenderer : public GenericMeshRenderer<SkinnedMesh> {

	public:
		SkinnedMeshRenderer(Transform& transform,
			SceneShader& shader,
			SkinnedModel& model,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "SkinnedMeshRenderer");

		virtual ~SkinnedMeshRenderer();

		Skeleton& getSkeleton();
		SkinnedModel& getSkinnedModel();

		virtual RenderMode getRenderMode() const;

	private:
		Skeleton* skeleton;
		SkinnedModel& skinnedModel;

	};

}

#endif
