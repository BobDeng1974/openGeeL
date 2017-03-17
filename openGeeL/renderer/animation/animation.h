#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <string>
#include <vector>
#include <vec3.hpp>
#include "skeleton.h"

namespace geeL{

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
	};


	class Animation {

	public:
		Animation(std::string name, double duration, double fps);
		~Animation();

		void addBoneData(std::string name, AnimationBoneData* data);
		Transform getFrame(const std::string& bone, double time) const;

		//Updates bone with transformational data from given time code
		void updateBone(const std::string& name, Transform& bone, double time) const;

		double getDuration() const;
		double getFPS() const;

		std::map<std::string, AnimationBoneData*>::const_iterator bonesStart() const;
		std::map<std::string, AnimationBoneData*>::const_iterator bonesEnd() const;

	private:
		std::string name;
		double duration;
		double fps;
		std::map<std::string, AnimationBoneData*> bones;

		glm::vec3 getVector(const std::vector<KeyFrame>& list, double time) const;
	};

}

#endif

