#include "../utility/vector3.h"
#include "../transformation/transform.h"
#include "animation.h"

using namespace glm;

namespace geeL {

	Animation::Animation(std::string name, double duration, double fps)
		: name(name), duration(duration), fps(fps) {}

	Animation::~Animation() {
		for (auto it = bones.begin(); it != bones.end(); it++) {
			AnimationBoneData* data = (*it).second;
			delete data;
		}
	}


	void Animation::addBoneData(std::string name, AnimationBoneData* data) {
		bones[name] = data;
	}

	Transform Animation::getFrame(const std::string& bone, double time) const {
		if (bones.find(bone) != bones.end()) {
			const AnimationBoneData& data = *bones.at(bone);

			auto positions = data.positions;
			vec3& position = getVector(positions, time);

			auto rotations = data.rotations;
			vec3& rotation = getVector(rotations, time);

			auto scalings = data.scalings;
			vec3& scaling = getVector(scalings, time);

			return Transform(position, rotation, scaling);
		}
		
		return Transform();
	}


	vec3 Animation::getVector(std::vector<KeyFrame>& list, double time) const {

		//Linear search in sorted key frame list
		unsigned int start = 0;
		unsigned int end = list.size();

		while ((end - start) > 1) {
			unsigned int step = (start + end) / 2;
			KeyFrame& frame = list[step];
			
			if (frame.time > time)
				end = step;
			else
				start = step;
		}

		//Linear interpolate the two closest results
		vec3& startVec = list[start].value;
		vec3& endVec = list[end].value;
		double factor = (time - list[start].time) 
			/ (list[end].time - list[start].time);

		return AlgebraHelper::lerp(startVec, endVec, factor);
	}

	double Animation::getDuration() const {
		return duration;
	}

	double Animation::getFPS() const {
		return fps;
	}

	std::map<std::string, AnimationBoneData*>::const_iterator Animation::bonesStart() const {
		return bones.begin();
	}

	std::map<std::string, AnimationBoneData*>::const_iterator Animation::bonesEnd() const {
		return bones.end();
	}

}