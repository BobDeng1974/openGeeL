#include "../transformation/transform.h"
#include "../utility/rendertime.h"
#include "animation.h"
#include "skeleton.h"
#include "animatedobject.h"
#include "animator.h"

namespace geeL {

	Animator::Animator(AnimatedObject& object, Transform& modelTransform)
		: object(object), skeleton(new Skeleton(object.getSkeleton())), modelTransform(modelTransform) {
	
		//Add transform of skeleton to the transform of the actual model
		//and therefore into the whole scene structure. Thus, updating 
		//transform information will be taken care off
		modelTransform.AddChild(skeleton->getRootBone());
	}

	Animator::~Animator() {
		delete skeleton;
	}

	void Animator::lateUpdate() {
		object.updateBones(*skeleton);
	}

	void Animator::resetSkeleton() {
		modelTransform.RemoveChild(*skeleton->getRootBone());

		delete skeleton;
		skeleton = new Skeleton(object.getSkeleton());
		modelTransform.AddChild(*skeleton->getRootBone());
	}

	const Skeleton& Animator::getSkeleton() const {
		return *skeleton;
	}


	SimpleAnimator::SimpleAnimator(AnimatedObject& object, Transform& modelTransform)
		: Animator(object, modelTransform), currentAnimation(nullptr), currentTime(0.) {}


	void SimpleAnimator::update() {
		if (currentAnimation != nullptr && currentTime < currentAnimation->getDuration()) {
			currentTime += currentAnimation->getFPS() * Time::deltaTime;

			for (auto it = currentAnimation->bonesStart(); it != currentAnimation->bonesEnd(); it++) {
				const std::string& name = (*it).first;

				Transform& bone = *skeleton->getBone(name);
				currentAnimation->updateBone(name, bone, currentTime);
			}
		}
	}

	void SimpleAnimator::playAnimation(unsigned int index) {
		currentAnimation = &object.getAnimation(index);
	}

	void SimpleAnimator::stop() {
		currentAnimation = nullptr;
		currentTime = 0.;
		//resetSkeleton();
	}

}
