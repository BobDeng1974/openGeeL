#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace geeL {

	class Animation;
	class AnimationMemory;
	class Skeleton;

	//Abstract base class for all objects that shall be animated
	class AnimatedObject {

	public:
		AnimatedObject();
		virtual ~AnimatedObject();

		//Add animation. Memory will be managed by this model
		virtual void addAnimation(std::unique_ptr<AnimationMemory> animation);

		//Get animation with given name. Returns nullptr if no such animation exists
		const AnimationMemory* const getAnimation(const std::string& name = "Default") const;
		void iterateAnimations(std::function<void(const Animation&)> function) const;

		const Skeleton& getSkeleton() const;
		void setSkeleton(std::unique_ptr<Skeleton> skeleton);

	protected:
		Skeleton* skeleton;
		std::map<std::string, AnimationMemory*> animations;
	};
}

#endif
