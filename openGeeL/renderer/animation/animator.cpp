#include "transformation/transform.h"
#include "utility/rendertime.h"
#include "bone.h"
#include "skeleton.h"
#include "animatedobject.h"
#include "animator.h"

namespace geeL {

	Animator::Animator(AnimatedObject& object, Skeleton& skeleton)
		: object(object)
		, skeleton(skeleton) {}


	SimpleAnimator::SimpleAnimator(AnimatedObject& object, Skeleton& skeleton)
		: Animator(object, skeleton)
		, currentAnimation(nullptr)
		, currentTime(0.) {}


	void SimpleAnimator::update(Input& input) {
		if (isRunning()) {
			currentTime += currentAnimation->getFPS() * RenderTime::deltaTime();
			currentAnimation->updateBones(currentTime);
		}
		else
			stop();

	}

	void SimpleAnimator::startAnimation(const std::string& name) {
		const AnimationMemory* memory = object.getAnimation(name);

		if(memory != nullptr)
			currentAnimation.reset(new AnimationInstance(*memory, skeleton));
	}

	void SimpleAnimator::stop() {
		currentAnimation.reset(nullptr);
		currentTime = 0.;
	}

	void SimpleAnimator::reset() {
		currentAnimation.reset(new AnimationInstance(*object.getAnimation("Default"), skeleton));
		currentTime = 0.;
	}

	bool SimpleAnimator::isRunning() const {
		return currentAnimation != nullptr && currentTime < currentAnimation->getDuration();
	}

}
