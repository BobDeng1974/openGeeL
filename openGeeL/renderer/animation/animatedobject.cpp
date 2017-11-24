#include "skeleton.h"
#include "animation.h"
#include "animatedobject.h"

namespace geeL {

	AnimatedObject::AnimatedObject() : skeleton(nullptr) {}

	AnimatedObject::~AnimatedObject() {
		if (skeleton != nullptr)
			delete skeleton;

		for (auto it = animations.begin(); it != animations.end(); it++)
			delete *it;
	}


	void AnimatedObject::addAnimation(std::unique_ptr<Animation> animation) {
		animations.push_back(animation.release());
	}

	const Animation& AnimatedObject::getAnimation(size_t index) const {
		size_t i = (index > animations.size() - 1) ? animations.size() - 1 : index;

		return *animations[i];
	}

	Skeleton& AnimatedObject::getSkeleton() const {
		return *skeleton;
	}

	void AnimatedObject::setSkeleton(std::unique_ptr<Skeleton> skeleton) {
		if (skeleton != nullptr)
			this->skeleton = skeleton.release();
	}

}