#include "model.h"
#include "meshrenderer.h"

namespace geeL {

	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform, Shader& shader,
		CullingMode faceCulling, bool deferred, std::string name)
			: MeshRenderer(transform, shader, faceCulling, deferred, name) {}

	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform, Shader& shader, SkinnedModel& model,
		CullingMode faceCulling, bool deferred, std::string name)
			: MeshRenderer(transform, shader, model, faceCulling, deferred, name), skinnedModel(&model) {}


}