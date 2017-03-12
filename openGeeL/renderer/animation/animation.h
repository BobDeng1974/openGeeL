#ifndef ANIMATION_H
#define ANIMATION_H

#include <list>
#include <map>
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

		std::list<KeyFrame> positions;
		std::list<KeyFrame> rotations;
		std::list<KeyFrame> scalings;
	};


	class Animation {

	public:
		Animation(std::string name, double duration, double fps);
		~Animation();

		void addBoneData(std::string name, AnimationBoneData* data);

	private:
		std::string name;
		double duration;
		double fps;
		std::map<std::string, AnimationBoneData*> bones;

	};

}

#endif

