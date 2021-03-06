#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <memory>
#include "scripting/component.h"
#include "animation.h"

namespace geeL {

	class AnimationContainer;
	class SceneObject;
	class Skeleton;
	class Transform;


	//Abstract base class for all animating objects
	class Animator : public Component {

	public:
		Animator(AnimationContainer& object, Skeleton& skeleton);

		virtual void reset() = 0;
		virtual bool isRunning() const = 0;

	protected:
		//Tick function of animator. Should be called every frame
		virtual void update(Input& input) = 0;

		AnimationContainer& object;
		Skeleton& skeleton;
	};


	//Animator that can playback single animations
	class SimpleAnimator : public Animator {

	public:
		SimpleAnimator(AnimationContainer& object, Skeleton& skeleton);
		
		void startAnimation(const std::string& name, double startTime = 0.);
		void stop();

		virtual void reset();
		virtual bool isRunning() const;

		void loopAnimation(bool loop);

	protected:
		virtual void update(Input& input);

	private:
		std::unique_ptr<AnimationInstance> currentAnimation;
		double currentTime;
		double startTime;
		bool looping;

	};
}

#endif