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
			StaticModel& model,
			CullingMode faceCulling = CullingMode::cullFront, 
			const std::string& name = "MeshRenderer");

		StaticMeshRenderer(Transform& transform,
			SceneShader& shader,
			std::list<const StaticMesh*>& meshes,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "MeshRenderer");

		virtual ~StaticMeshRenderer();


		virtual RenderMode getRenderMode() const;

	private:
		std::list<MeshInstance*> meshes;

		void initMaterials(SceneShader& shader, StaticModel& model);
		void initMaterials(SceneShader& shader, std::list<const StaticMesh*>& meshes);

	};


	//Special mesh renderer that is intended for use with animated/skinned models
	class SkinnedMeshRenderer : public MeshRenderer {

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
		std::list<MeshInstance*> meshes;
		Skeleton* skeleton;
		SkinnedModel& skinnedModel;

		void initMaterials(SceneShader& shader);

	};

}

#endif
