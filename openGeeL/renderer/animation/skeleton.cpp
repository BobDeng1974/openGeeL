#include "transformation/transform.h"
#include "skeleton.h"

namespace geeL {

	Skeleton::Skeleton(Transform* const root) : rootBone(root) {
		addBone(root);
	}

	Skeleton::~Skeleton() {
		//Only delete rootBone if it isn't part of any transformation structure
		//since it will then be deleted automatically
		if(rootBone->GetParent() == nullptr)
			delete rootBone;
	}


	unsigned int Skeleton::getBoneID(std::string name) const {
		auto it = bones.find(name);
		if (it != bones.end())
			return it->second->getID();

		return 0;
	}

	Transform* const Skeleton::getRootBone() {
		return rootBone;
	}

	Transform* const Skeleton::getBone(std::string name) {
		auto it = bones.find(name);
		if (it != bones.end())
			return it->second;

		return nullptr;
	}

	const Transform* const Skeleton::getBone(std::string name) const {
		auto it = bones.find(name);
		if (it != bones.end())
			return it->second;

		return nullptr;
	}

	void Skeleton::setBone(std::string name, const Transform& transform) {
		auto it = bones.find(name);
		if (it != bones.end()) {
			Transform* trans = it->second;
			trans->setMatrix(transform.getMatrix());
		}
	}

	void Skeleton::addBone(Transform* transform) {
		bones[transform->getName()] = transform;

		for (auto it = transform->childrenStart(); it != transform->childrenEnd(); it++)
			addBone(*it);
	}

}