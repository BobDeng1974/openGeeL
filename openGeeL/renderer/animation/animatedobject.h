#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include <map>
#include <memory>
#include <string>

namespace geeL {

	class Animation;
	class Skeleton;

	//Abstract base class for all objects that shall be animated
	class AnimatedObject {

	public:
		AnimatedObject();
		virtual ~AnimatedObject();

		//Add animation. Memory will be managed by this model
		virtual void addAnimation(std::unique_ptr<Animation> animation);

		//Get animation with given name. Returns nullptr if no such animation exists
		const Animation* const getAnimation(const std::string& name = "Default") const;

		const Skeleton& getSkeleton() const;
		void setSkeleton(std::unique_ptr<Skeleton> skeleton);

	protected:
		Skeleton* skeleton;
		std::map<std::string, Animation*> animations;
	};
}

#endif
