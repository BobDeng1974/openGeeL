#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "../scripting/component.h"

namespace geeL {

	class AnimatedObject;
	class Animation;
	class SceneObject;
	class Skeleton;
	class Transform;

	//Abstract base class for all animating objects
	class Animator : public Component {

	public:
		Animator(AnimatedObject& object, Skeleton& skeleton);

		const Skeleton& getSkeleton() const;

	protected:
		virtual void resetSkeleton();

		//Tick function of animator. Should be called every frame
		virtual void update() = 0;

		AnimatedObject& object;
		Skeleton& skeleton;
	};


	//Animator that can playback single animations
	class SimpleAnimator : public Animator {

	public:
		SimpleAnimator(AnimatedObject& object, Skeleton& skeleton);
		
		void playAnimation(unsigned int index);
		void stop();

	protected:
		virtual void update();

	private:
		const Animation* currentAnimation;
		double currentTime;

	};
}

#endif