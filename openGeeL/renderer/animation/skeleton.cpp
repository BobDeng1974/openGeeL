#include "skeleton.h"

namespace geeL {

	Skeleton::Skeleton(Transform* root) : rootBone(root) {
		addBone(root);
	}


	Skeleton::~Skeleton() {
		delete rootBone;
	}

	const Transform* Skeleton::getBone(std::string name) const {
		auto it = bones.find(name);
		if (it != bones.end())
			return it->second;

		return nullptr;
	}

	void Skeleton::setBone(std::string name, const Transform& transform) {
		auto it = bones.find(name);
		if (it != bones.end()) {
			Transform* trans = it->second;
			trans->matrix = transform.matrix;
		}
	}

	void Skeleton::addBone(Transform* transform) {
		bones[transform->getName()] = transform;

		for (auto it = transform->childrenStart(); it != transform->childrenEnd(); it++)
			addBone(*it);
	}

}