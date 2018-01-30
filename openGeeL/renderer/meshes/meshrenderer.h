#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "instancedmesh.h"
#include "ameshrenderer.h"

namespace geeL {

	class StaticModel;
	class SkinnedModel;
	class Skeleton;


	//Represents a drawn model in a render scene. Independent from actual model
	class StaticMeshRenderer : public MeshRenderer {

	public:
		//Constructor for mesh renderer with an unique assigned model
		StaticMeshRenderer(Transform& transform, 
			SceneShader& shader, 
			MemoryObject<StaticModel> model,
			CullingMode faceCulling = CullingMode::cullFront, 
			const std::string& name = "MeshRenderer");

		StaticMeshRenderer(Transform& transform,
			SceneShader& shader,
			MemoryObject<StaticModel> modelData,
			std::list<const StaticMesh*>& meshes,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "MeshRenderer");

		virtual RenderMode getRenderMode() const;

	private:
		void initialize(SceneShader& shader, StaticModel& model);
		void initialize(SceneShader& shader, std::list<const StaticMesh*>& meshes);

	};


	//Special mesh renderer that is intended for use with animated/skinned models
	class SkinnedMeshRenderer : public MeshRenderer {

	public:
		SkinnedMeshRenderer(Transform& transform,
			SceneShader& shader,
			MemoryObject<SkinnedModel> model,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "SkinnedMeshRenderer");

		virtual ~SkinnedMeshRenderer();

		Skeleton& getSkeleton();
		SkinnedModel& getSkinnedModel();

		virtual RenderMode getRenderMode() const;

	private:
		Skeleton* skeleton;
		SkinnedModel& skinnedModel;

		void initMaterials(SceneShader& shader);

	};

}

#endif
