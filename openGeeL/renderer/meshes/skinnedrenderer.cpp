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


	void SkinnedMeshRenderer::draw(SceneShader& shader) const {
		CullingGuard culling(faceCulling);

		//TODO: implement this
		/*
		for (auto it = materials->begin(); it != materials->end(); it++) {
		Material& material = *it->second;
		const RenderShader& shader = material.getShader();

		//Load materials into shader
		material.bindTextures();
		material.bind();

		unsigned int index = it->first;
		const SkinnedMesh& mesh = skinnedModel->getSkinnedMesh(index);

		//Bind bone transforms to indivdual meshes
		for (auto et = mesh.bonesBeginConst(); et != mesh.bonesEndBegin(); et++) {
		const MeshBoneData& data = et->second;
		const glm::mat4& transform = data.transform;

		shader.bind<glm::mat4>("bones[" + std::to_string(data.id) + "]", transform);
		}

		//Draw individual mesh
		mesh.draw();
		}
		*/
	}

	void SkinnedMeshRenderer::drawExclusive(SceneShader& shader) const {
		//TODO: implement this
	}

	void SkinnedMeshRenderer::drawGeometry(const RenderShader& shader) const {
		//TODO: Load relevant bone transforms into materials
		transform.bind(shader, "model");


		skinnedModel->draw();
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