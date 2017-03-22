#include "../transformation/transform.h"
#include "../utility/rendertime.h"
#include "animation.h"
#include "skeleton.h"
#include "animatedobject.h"
#include "animator.h"
#include <iostream>

namespace geeL {

	Animator::Animator(AnimatedObject& object, Skeleton& skeleton)
		: object(object), skeleton(skeleton) {}


	void Animator::resetSkeleton() {
		//TODO: implement this
	}

	const Skeleton& Animator::getSkeleton() const {
		return skeleton;
	}


	SimpleAnimator::SimpleAnimator(AnimatedObject& object, Skeleton& skeleton)
		: Animator(object, skeleton), currentAnimation(nullptr), currentTime(0.) {}


	void SimpleAnimator::update() {
		if (currentAnimation != nullptr && currentTime < currentAnimation->getDuration()) {
			currentTime += currentAnimation->getFPS() * Time::deltaTime;

			for (auto it = currentAnimation->bonesStart(); it != currentAnimation->bonesEnd(); it++) {
				const std::string& name = (*it).first;

				Transform& bone = *skeleton.getBone(name);
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
