#include <iostream>
#include "../shader/shader.h"
#include "../materials/material.h"
#include "../transformation/transform.h"
#include "../animation/animator.h"
#include "../animation/skeleton.h"
#include "model.h"
#include "meshrenderer.h"

namespace geeL {

	SkinnedMeshRenderer::SkinnedMeshRenderer(Transform& transform, SceneShader& shader,
		CullingMode faceCulling, std::string name)
			: MeshRenderer(transform, shader, faceCulling, name), skinnedModel(nullptr) , skeleton(nullptr) {}

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


	void SkinnedMeshRenderer::transformMeshes(const Shader* shader) const {
		//TODO: bind actual skeleton instead of root transform
		shader->use();
		shader->setMat4("model", transform.getMatrix());
	}

	void SkinnedMeshRenderer::transformMeshes(Model& model, const std::map<unsigned int, Material*>& materials) const {

		//TODO: bind actual skeleton instead of root transform
		for (auto it = materials.begin(); it != materials.end(); it++) {
			Material* mat = it->second;
			const Shader& shader = mat->shader;

			shader.use();
			shader.setMat4("model", transform.getMatrix());
		}
	}

	Skeleton& SkinnedMeshRenderer::getSkeleton() {
		return *skeleton;
	}

	SkinnedModel& SkinnedMeshRenderer::getSkinnedModel() {
		return *skinnedModel;
	}

	template<class T>
	T& SkinnedMeshRenderer::addAnimatorComponent(const T& animator) {
		T* t = new T(animator);

		if (dynamic_cast<Animator*>(t) != nullptr) {
			this->animator = t;
			this->animator->init();
			this->components.push_back(animator);
		}
		else
			std::cout << "Given component is not an animator\n";
	}
}