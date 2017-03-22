#include "../utility/vectorextension.h"
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

			const std::vector<KeyFrame>& positions = data.positions;
			vec3& position = getVector(positions, time);

			const std::vector<KeyFrame>& rotations = data.rotations;
			vec3& rotation = getVector(rotations, time);

			const std::vector<KeyFrame>& scalings = data.scalings;
			vec3& scaling = getVector(scalings, time);

			return Transform(position, rotation, scaling);
		}
		
		return Transform();
	}

	void Animation::updateBone(const std::string& name, Transform& bone, double time) const {

		auto it = bones.find(name);
		if (it != bones.end()) {
			const AnimationBoneData& data = *(*it).second;

			const std::vector<KeyFrame>& positions = data.positions;
			vec3& position = getVector(positions, time);
			bone.setPosition(position);

			const std::vector<KeyFrame>& rotations = data.rotations;
			vec3& rotation = getVector(rotations, time);
			bone.setEulerAngles(rotation);

			const std::vector<KeyFrame>& scalings = data.scalings;
			vec3& scaling = getVector(scalings, time);
			bone.setScaling(scaling);
		}

	}

	vec3 Animation::getVector(const std::vector<KeyFrame>& list, double time) const {

		//Linear search in sorted key frame list
		size_t start = 0;
		size_t end = list.size();

		while ((end - start) > 1) {
			size_t step = (start + end) / 2;
			const KeyFrame& frame = list[step];
			
			if (frame.time > time)
				end = step;
			else
				start = step;
		}

		//Linear interpolate the two closest results
		const vec3& startVec = list[start].value;
		const vec3& endVec = list[end].value;
		double factor = (time - list[start].time) 
			/ (list[end].time - list[start].time);

		return VectorExtension::lerp(startVec, endVec, float(factor));
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