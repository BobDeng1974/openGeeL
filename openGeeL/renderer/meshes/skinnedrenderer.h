#ifndef SKINNEDMESHRENDERER_H
#define SKINNEDMESHRENDERER_H

#include "meshrenderer.h"

namespace geeL {

	//Special mesh renderer that is intended for use with animated/skinned models
	class SkinnedMeshRenderer : public MeshRenderer {

	public:
		SkinnedMeshRenderer(Transform& transform, SceneShader& shader, SkinnedModel& model,
			CullingMode faceCulling = CullingMode::cullFront, const std::string& name = "SkinnedMeshRenderer");

		~SkinnedMeshRenderer();

		//Updates model with current transformational data of skeleton
		virtual void lateUpdate();

		virtual void draw(bool deferred = true) const;
		virtual void draw(const SceneShader& shader) const;
		virtual void draw(const Shader& shader) const;

		Skeleton& getSkeleton();
		SkinnedModel& getSkinnedModel();

		virtual RenderMode getRenderMode() const;

	private:
		Skeleton* skeleton;
		SkinnedModel* skinnedModel;

		void loadSkeleton(const Shader& shader) const;
	};

}

#endif
