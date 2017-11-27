#include "utility/vectorextension.h"
#include "skeleton.h"
#include "animation.h"

using namespace glm;

namespace geeL {

	AnimationMemory::AnimationMemory(const std::string& name, double duration, double fps)
		: name(name), duration(duration), fps(fps) {}

	AnimationMemory::~AnimationMemory() {
		for (auto it = bones.begin(); it != bones.end(); it++) {
			const AnimationBoneData* data = (*it).second;
			delete data;
		}
	}


	void AnimationMemory::addBoneData(const std::string& name, std::unique_ptr<AnimationBoneData> data) {
		bones[name] = data.release();
	}

	void AnimationMemory::updateBone(const std::string& name, Transform& bone, double time) const {

		auto it = bones.find(name);
		if (it != bones.end()) {
			const AnimationBoneData& data = *(*it).second;
			data.updateBone(bone, time);
		}
	}

	const AnimationBoneData* const AnimationMemory::getAnimationData(const std::string& name) const {
		auto it(bones.find(name));
		if (it != bones.end())
			return it->second;

		return nullptr;
	}

	double AnimationMemory::getDuration() const {
		return duration;
	}

	double AnimationMemory::getFPS() const {
		return fps;
	}

	const std::string& AnimationMemory::getName() const {
		return name;
	}

	


	AnimationInstance::AnimationInstance(const AnimationMemory& animation, Skeleton& skeleton)
		: animation(animation)
		, skeleton(skeleton) {
	
		connectSkeleton();
	}


	void AnimationInstance::updateBone(const std::string& name, Transform& bone, double time) const {
		animation.updateBone(name, bone, time);
	}

	void AnimationInstance::updateBones(double time) {
		for (auto it(bones.begin()); it != bones.end(); it++) {
			Bone& bone = *it->first;
			const AnimationBoneData& data = *it->second;

			data.updateBone(bone, time);
		}
	}

	double AnimationInstance::getDuration() const {
		return animation.getDuration();
	}

	double AnimationInstance::getFPS() const {
		return animation.getFPS();
	}

	const std::string& AnimationInstance::getName() const {
		return animation.getName();
	}

	void AnimationInstance::connectSkeleton() {
		skeleton.iterateBones([this](Bone& bone) {
			const AnimationBoneData* data = animation.getAnimationData(bone.getName());

			if (data != nullptr)
				bones[&bone] = data;
		});
	}



	vec3 getVector(const std::vector<KeyFrame>& list, double time) {
		//assert(list.size() > 0);
		if (list.size() == 1) return list[0].value;

		//Binary search in sorted key frame list
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

		return VectorExtension::lerp(startVec, endVec, float(1. - factor));
	}

	void AnimationBoneData::updateBone(Bone& bone, double time) const {
		if (positions.size() > 0) {
			vec3& position = getVector(positions, time);
			bone.setPosition(position);
		}

		if (rotations.size() > 0) {
			vec3& rotation = getVector(rotations, time);
			bone.setEulerAngles(rotation);
		}

		if (scalings.size() > 0) {
			vec3& scaling = getVector(scalings, time);
			bone.setScaling(scaling);
		}
	}

}