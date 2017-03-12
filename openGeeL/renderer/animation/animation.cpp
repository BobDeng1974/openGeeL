#include "animation.h"

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


}