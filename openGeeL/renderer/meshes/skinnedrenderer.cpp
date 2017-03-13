#include "model.h"
#include "meshrenderer.h"

namespace geeL {

	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform, SceneShader& shader,
		CullingMode faceCulling, std::string name)
			: MeshRenderer(transform, shader, faceCulling, name) {}

	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform, SceneShader& shader, SkinnedModel& model,
		CullingMode faceCulling, std::string name)
			: MeshRenderer(transform, shader, model, faceCulling, name), skinnedModel(&model) {}


}