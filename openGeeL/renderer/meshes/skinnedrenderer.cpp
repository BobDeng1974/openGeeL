#include <iostream>
#include "../shader/shader.h"
#include "../materials/material.h"
#include "../transformation/transform.h"
#include "../animation/animator.h"
#include "../animation/skeleton.h"
#include "model.h"
#include "meshrenderer.h"

namespace geeL {

	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform, SceneShader& shader, SkinnedModel& model,
		CullingMode faceCulling, std::string name)
			: MeshRenderer(transform, shader, model, faceCulling, name), 
				skinnedModel(&model), skeleton(new Skeleton(model.getSkeleton())) {
	
		//Add transform of skeleton to the transform of the actual model
		//and therefore into the whole scene structure. Thus, updating 
		//transform information will be taken care off
		transform.AddChild(skeleton->getRootBone());
	}

	SkinnedMeshRenderer::~SkinnedMeshRenderer() {
		if (skeleton != nullptr)
			delete skeleton;
	}


	void SkinnedMeshRenderer::lateUpdate() {
		skinnedModel->updateBones(*skeleton);
	}

	void SkinnedMeshRenderer::draw(bool deferred) const {
		cullFaces();

		const std::map<unsigned int, Material*>* materials =
			deferred ? &deferredMaterials : &forwardMaterials;

		for (auto it = materials->begin(); it != materials->end(); it++) {
			Material& material = *it->second;
			Shader& shader = material.shader;
			shader.use();

			//Load materials into shader
			material.bindTextures();
			material.bind();

			unsigned int index = it->first;
			const SkinnedMesh& mesh = skinnedModel->getSkinnedMesh(index);

			//Bind bone transforms for indivdual meshes
			for (auto et = mesh.bonesBeginConst(); et != mesh.bonesEndBegin(); et++) {
				const MeshBoneData& data = et->second;
				const glm::mat4& transform = data.transform;

				shader.setMat4("bones[" + std::to_string(data.id) + "]", transform);
			}

			//Draw individual mesh
			mesh.draw();
		}

		uncullFaces();
	}

	void SkinnedMeshRenderer::draw(const Shader& shader) const {
		cullFaces();

		//TODO: Load relevant bone transforms into materials

		//Draw model
		transformMeshes(&shader);
		skinnedModel->draw();

		uncullFaces();
	}

	Skeleton& SkinnedMeshRenderer::getSkeleton() {
		return *skeleton;
	}

	SkinnedModel& SkinnedMeshRenderer::getSkinnedModel() {
		return *skinnedModel;
	}

	void SkinnedMeshRenderer::loadSkeleton(const Shader& shader) const {
		skinnedModel->iterateMeshes([&](const SkinnedMesh* mesh) {
			for (auto et = mesh->bonesBeginConst(); et != mesh->bonesEndBegin(); et++) {
				const MeshBoneData& data = et->second;
				const glm::mat4& transform = data.transform;

				shader.setMat4("bones[" + std::to_string(data.id) + "]", transform);
			}
		});
	}

	RenderMode SkinnedMeshRenderer::getRenderMode() const {
		return RenderMode::Skinned;
	}
}