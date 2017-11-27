#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <memory>
#include "scripting/component.h"
#include "animation.h"

namespace geeL {

	class AnimatedObject;
	class SceneObject;
	class Skeleton;
	class Transform;


	//Abstract base class for all animating objects
	class Animator : public Component {

	public:
		Animator(AnimatedObject& object, Skeleton& skeleton);

		virtual void reset() = 0;
		virtual bool isRunning() const = 0;

	protected:
		//Tick function of animator. Should be called every frame
		virtual void update(Input& input) = 0;

		AnimatedObject& object;
		Skeleton& skeleton;
	};


	//Animator that can playback single animations
	class SimpleAnimator : public Animator {

	public:
		SimpleAnimator(AnimatedObject& object, Skeleton& skeleton);
		
		void startAnimation(const std::string& name);
		void stop();

		virtual void reset();
		virtual bool isRunning() const;

	protected:
		virtual void update(Input& input);

	private:
		std::unique_ptr<AnimationInstance> currentAnimation;
		double currentTime;

	};
}

#endif