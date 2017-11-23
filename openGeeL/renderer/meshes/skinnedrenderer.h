#ifndef SKINNEDMESHRENDERER_H
#define SKINNEDMESHRENDERER_H

#include "meshrenderer.h"

namespace geeL {

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
		Skeleton* skeleton;
		SkinnedModel* skinnedModel;

	};

}

#endif

