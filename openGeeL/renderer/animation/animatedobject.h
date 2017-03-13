#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include <vector>

namespace geeL {

	class Animation;
	class Skeleton;

	//Abstract base class for all objects that shall be animated
	class AnimatedObject {

	public:
		~AnimatedObject();

		//Add animation. Memory will be managed by this model
		virtual void addAnimation(Animation* animation);

		//Get animation at index. Index will get clamped if not present.
		const Animation& getAnimation(unsigned int index) const;
		const Skeleton& getSkeleton() const;

	protected:
		AnimatedObject() {}

		Skeleton* skeleton;
		std::vector<Animation*> animations;
	};

}


#endif
