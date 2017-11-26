#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "scripting/component.h"

namespace geeL {

	class AnimatedObject;
	class AnimationMemory;
	class SceneObject;
	class Skeleton;
	class Transform;

	//Abstract base class for all animating objects
	class Animator : public Component {

	public:
		Animator(AnimatedObject& object, Skeleton& skeleton);

	protected:
		//Tick function of animator. Should be called every frame
		virtual void update(Input& input) = 0;
		virtual void reset() = 0;

		AnimatedObject& object;
		Skeleton& skeleton;
	};


	//Animator that can playback single animations
	class SimpleAnimator : public Animator {

	public:
		SimpleAnimator(AnimatedObject& object, Skeleton& skeleton);
		
		void playAnimation(const std::string& name);
		void stop();

	protected:
		virtual void update(Input& input);
		virtual void reset();

	private:
		const AnimationMemory* currentAnimation;
		double currentTime;

	};
}

#endif