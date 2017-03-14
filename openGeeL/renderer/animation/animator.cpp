#include "../transformation/transform.h"
#include "../utility/rendertime.h"
#include "animation.h"
#include "skeleton.h"
#include "animatedobject.h"
#include "animator.h"

namespace geeL {

	Animator::Animator(const AnimatedObject& object) 
		: object(object), skeleton(new Skeleton(object.getSkeleton())) {}

	Animator::~Animator() {
		delete skeleton;
	}

	void Animator::resetSkeleton() {
		delete skeleton;
		skeleton = new Skeleton(object.getSkeleton());
	}


	SimpleAnimator::SimpleAnimator(const AnimatedObject& object) 
		: Animator(object), currentAnimation(nullptr), currentTime(0.) {}


	void SimpleAnimator::update() {
		if (currentAnimation != nullptr) {
			currentTime = currentAnimation->getFPS() * Time::deltaTime;

			for (auto it = currentAnimation->bonesStart(); it != currentAnimation->bonesEnd(); it++) {
				const std::string& name = (*it).first;

				Transform transform = currentAnimation->getFrame(name, currentTime);
				skeleton->setBone(name, transform);

				//TODO: incooperate parent transform
			}
		}
	}

	void SimpleAnimator::playAnimation(unsigned int index) {
		currentAnimation = &object.getAnimation(index);
	}

	void SimpleAnimator::stop() {
		currentAnimation = nullptr;
		currentTime = 0.;
		resetSkeleton();
	}

}
