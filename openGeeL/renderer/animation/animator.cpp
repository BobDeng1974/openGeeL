#include "transformation/transform.h"
#include "utility/rendertime.h"
#include "animation.h"
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
		if (currentAnimation != nullptr && currentTime <= currentAnimation->getDuration()) {
			currentTime += currentAnimation->getFPS() * RenderTime::deltaTime();

			skeleton.iterateBones([this](Bone& bone) {
				currentAnimation->updateBone(bone.getName(), bone, currentTime);
			});
		}
	}

	void SimpleAnimator::playAnimation(const std::string& name) {
		currentAnimation = object.getAnimation(name);
	}

	void SimpleAnimator::stop() {
		currentAnimation = nullptr;
		currentTime = 0.;
	}

	void SimpleAnimator::reset() {
		currentAnimation = object.getAnimation("Default");
		currentTime = 0.;
	}

}
