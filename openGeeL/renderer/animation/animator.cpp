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
		if (currentAnimation != nullptr && currentTime <= currentAnimation->getDuration()) {
			currentTime += currentAnimation->getFPS() * RenderTime::deltaTime();
			currentAnimation->updateBones(currentTime);

			/*
			skeleton.iterateBones([this](Bone& bone) {
				currentAnimation->updateBone(bone.getName(), bone, currentTime);
			});
			*/
		}
	}

	void SimpleAnimator::playAnimation(const std::string& name) {
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

}
