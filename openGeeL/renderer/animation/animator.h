#ifndef ANIMATOR_H
#define ANIMATOR_H


namespace geeL {

	class AnimatedObject;
	class Animation;
	class Skeleton;
	class Transform;

	//Abstract base class for all animating objects
	class Animator {

	public:
		//Tick function of animator. Should be called every frame
		virtual void update() = 0;

	protected:
		Animator(const AnimatedObject& object, Transform& modelTransform);
		~Animator();
		
		virtual void resetSkeleton();

		const AnimatedObject& object;
		Skeleton* skeleton;
		Transform& modelTransform;
	};


	//Animator that can playback single animations
	class SimpleAnimator : public Animator {

	public:
		SimpleAnimator(const AnimatedObject& object, Transform& modelTransform);

		virtual void update();
		void playAnimation(unsigned int index);
		void stop();

	private:
		const Animation* currentAnimation;
		double currentTime;

	};

}

#endif