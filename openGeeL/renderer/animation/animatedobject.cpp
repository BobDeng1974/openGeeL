#include "skeleton.h"
#include "animation.h"
#include "animatedobject.h"

namespace geeL {

	AnimatedObject::~AnimatedObject() {
		for (auto it = animations.begin(); it != animations.end(); it++)
			delete *it;
	}


	void AnimatedObject::addAnimation(Animation* animation) {
		animations.push_back(animation);
	}

	const Animation& AnimatedObject::getAnimation(unsigned int index) const {
		unsigned int i = (index > animations.size() - 1) ? animations.size() - 1 : index;

		return *animations[i];
	}

	const Skeleton& AnimatedObject::getSkeleton() const {
		return *skeleton;
	}

}