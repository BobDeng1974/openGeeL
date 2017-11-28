#include "skeleton.h"
#include "animation.h"
#include "animcontainer.h"

namespace geeL {

	AnimationContainer::AnimationContainer() : skeleton(nullptr) {}

	AnimationContainer::~AnimationContainer() {
		if (skeleton != nullptr)
			delete skeleton;

		for (auto it = animations.begin(); it != animations.end(); it++)
			delete it->second;
	}


	void AnimationContainer::addAnimation(std::unique_ptr<AnimationMemory> animation) {
		AnimationMemory* a = animation.release();

		animations[a->getName()] = a;
	}

	const AnimationMemory * const AnimationContainer::getAnimation(const std::string& name) const {
		auto it(animations.find(name));
		if (it != animations.end())
			return it->second;

		return nullptr;
	}

	void AnimationContainer::iterateAnimations(std::function<void(const Animation&)> function) const {
		for (auto it = animations.begin(); it != animations.end(); it++) {
			const Animation& animation = *it->second;
			function(animation);
		}
	}

	const Skeleton& AnimationContainer::getSkeleton() const {
		return *skeleton;
	}

	void AnimationContainer::setSkeleton(std::unique_ptr<Skeleton> skeleton) {
		if (skeleton != nullptr)
			this->skeleton = skeleton.release();
	}

}