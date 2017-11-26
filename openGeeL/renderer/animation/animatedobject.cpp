#include "skeleton.h"
#include "animation.h"
#include "animatedobject.h"

namespace geeL {

	AnimatedObject::AnimatedObject() : skeleton(nullptr) {}

	AnimatedObject::~AnimatedObject() {
		if (skeleton != nullptr)
			delete skeleton;

		for (auto it = animations.begin(); it != animations.end(); it++)
			delete it->second;
	}


	void AnimatedObject::addAnimation(std::unique_ptr<AnimationMemory> animation) {
		AnimationMemory* a = animation.release();

		animations[a->getName()] = a;
	}

	const AnimationMemory * const AnimatedObject::getAnimation(const std::string& name) const {
		auto it(animations.find(name));
		if (it != animations.end())
			return it->second;

		return nullptr;
	}

	const Skeleton& AnimatedObject::getSkeleton() const {
		return *skeleton;
	}

	void AnimatedObject::setSkeleton(std::unique_ptr<Skeleton> skeleton) {
		if (skeleton != nullptr)
			this->skeleton = skeleton.release();
	}

}