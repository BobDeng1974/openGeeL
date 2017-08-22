#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include <vector>

namespace geeL {

	class Animation;
	class Skeleton;

	//Abstract base class for all objects that shall be animated
	class AnimatedObject {

	public:
		AnimatedObject();
		virtual ~AnimatedObject();

		//Add animation. Memory will be managed by this model
		virtual void addAnimation(Animation* animation);

		//Update object data according to given animated skeleton
		virtual void updateBones(Skeleton& skeleton) = 0;

		//Get animation at index. Index will get clamped if not present.
		const Animation& getAnimation(size_t index) const;

		const Skeleton& getSkeleton() const;

		//Set skeleton for this animated object. It is assumed that the 
		//skeleton is completely processed and ready to use
		virtual void setSkeleton(Skeleton* const skeleton);

	protected:
		Skeleton* skeleton;
		std::vector<Animation*> animations;
	};
}

#endif
