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


	void SkinnedMeshRenderer::draw(bool deferred) const {
		cullFaces();

		//Draw model
		const std::map<unsigned int, Material*>* materials =
			deferred ? &deferredMaterials : &forwardMaterials;

		//TODO: load skeleton matrices into shader
		transformMeshes(*skinnedModel, *materials);
		skinnedModel->draw(*materials);

		uncullFaces();
	}

	void SkinnedMeshRenderer::draw(const Shader& shader) const {
		cullFaces();

		//Draw model
		//TODO: load skeleton matrices into shader
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