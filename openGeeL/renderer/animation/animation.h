#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <vec3.hpp>
#include "bone.h"
#include "transformation/transform.h"

namespace geeL{

	class Skeleton;


	class Animation {

	public:
		virtual ~Animation() {}

		//Updates bone with transformational data from given time code
		virtual void updateBone(const std::string& name, Transform& bone, double time) const = 0;

		virtual double getDuration() const = 0;
		virtual double getFPS() const = 0;
		virtual const std::string& getName() const = 0;

	};


	struct KeyFrame {
		glm::vec3 value;
		double time;

		KeyFrame(const glm::vec3& value, double time)
			: value(value), time(time) {}
	};

	struct AnimationBoneData {

		std::vector<KeyFrame> positions;
		std::vector<KeyFrame> rotations;
		std::vector<KeyFrame> scalings;

		void updateBone(Bone& bone, double time) const;

	};


	class AnimationMemory : public Animation {

	public:
		AnimationMemory(const std::string& name, double duration, double fps);

		virtual ~AnimationMemory();

		void addBoneData(const std::string& name, std::unique_ptr<AnimationBoneData> data);
		virtual void updateBone(const std::string& name, Transform& bone, double time) const;

		const AnimationBoneData* const getAnimationData(const std::string& name) const;

		virtual double getDuration() const;
		virtual double getFPS() const;
		virtual const std::string& getName() const;

	private:
		std::string name;
		double duration;
		double fps;
		std::map<std::string, const AnimationBoneData*> bones;

	};



	class AnimationInstance : public Animation {

	public:
		AnimationInstance(const AnimationMemory& animation, Skeleton& skeleton);

		virtual void updateBone(const std::string& name, Transform& bone, double time) const;

		//Update all bones of comprised skeleton 
		//with data from comprised animation
		void updateBones(double time);

		virtual double getDuration() const;
		virtual double getFPS() const;
		virtual const std::string& getName() const;

	private:
		const AnimationMemory& animation;
		Skeleton& skeleton;
		std::map<Bone*, const AnimationBoneData*> bones;

		//Connect skeleton instance to bone data of animation
		void connectSkeleton();

	};

}

#endif

