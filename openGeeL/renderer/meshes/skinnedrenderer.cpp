#include <iostream>
#include "../shader/sceneshader.h"
#include "../materials/material.h"
#include "../transformation/transform.h"
#include "../animation/skeleton.h"
#include "model.h"
#include "skinnedrenderer.h"

namespace geeL {

	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform, SceneShader& shader, SkinnedModel& model,
		CullingMode faceCulling, const std::string& name)
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

		/*
		for (auto it = materials->begin(); it != materials->end(); it++) {
			Material& material = *it->second;
			const Shader& shader = material.getShader();
			shader.use();

			//Load materials into shader
			material.bindTextures();
			material.bind();

			unsigned int index = it->first;
			const SkinnedMesh& mesh = skinnedModel->getSkinnedMesh(index);

			//Bind bone transforms to indivdual meshes
			for (auto et = mesh.bonesBeginConst(); et != mesh.bonesEndBegin(); et++) {
				const MeshBoneData& data = et->second;
				const glm::mat4& transform = data.transform;

				shader.setMat4("bones[" + std::to_string(data.id) + "]", transform);
			}

			//Draw individual mesh
			mesh.draw();
		}
		*/
		uncullFaces();
	}

	void SkinnedMeshRenderer::draw(const SceneShader& shader) const {
		cullFaces();

		//TODO: implement this

		uncullFaces();
	}

	void SkinnedMeshRenderer::draw(const Shader& shader) const {
		cullFaces();

		//TODO: Load relevant bone transforms into materials
		shader.use();
		shader.setMat4("model", transform.getMatrix());

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
		skinnedModel->iterateMeshes([&](const SkinnedMesh& mesh) {
			for (auto et = mesh.bonesBeginConst(); et != mesh.bonesEndBegin(); et++) {
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