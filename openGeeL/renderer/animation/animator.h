#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "../scripting/component.h"

namespace geeL {

	class AnimatedObject;
	class Animation;
	class Skeleton;
	class Transform;

	//Abstract base class for all animating objects
	class Animator : Component {

	public:
		//Tick function of animator. Should be called every frame
		virtual void update() = 0;

		//Second tick function that should be called after transformations
		//for current frame are computed since it forwards final transformations
		//into animated object
		virtual void lateUpdate();

		const Skeleton& getSkeleton() const;

	protected:
		Animator(AnimatedObject& object, Transform& modelTransform);
		~Animator();
		
		virtual void resetSkeleton();

		AnimatedObject& object;
		Skeleton* skeleton;
		Transform& modelTransform;
	};


	//Animator that can playback single animations
	class SimpleAnimator : public Animator {

	public:
		SimpleAnimator(AnimatedObject& object, Transform& modelTransform);

		virtual void update();
		void playAnimation(unsigned int index);
		void stop();

	private:
		const Animation* currentAnimation;
		double currentTime;

	};
}

#endif