#include "transformation/transform.h"
#include "skeleton.h"

namespace geeL {
	
	Skeleton::Skeleton() : rootBone(nullptr) {}

	Skeleton::Skeleton(Transform& root) : rootBone(&root) {
		addBone(&root);
	}

	Skeleton::Skeleton(const Skeleton& other) 
		: rootBone(new Transform(*other.rootBone)) {

		addBone(rootBone);
	}

	Skeleton::Skeleton(Skeleton&& other) 
		: rootBone(other.rootBone)
		, bones(std::move(other.bones)) {

		other.rootBone = nullptr;
	}

	Skeleton::~Skeleton() {
		//Only delete rootBone if it isn't part of any transformation structure
		//since it will then be deleted automatically
		if(rootBone != nullptr && rootBone->GetParent() == nullptr)
			delete rootBone;
	}

	Skeleton& Skeleton::operator=(Skeleton&& other) {
		if (this != &other) {
			rootBone = other.rootBone;
			bones = std::move(other.bones);

			other.rootBone = nullptr;
		}

		return *this;
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

	void Skeleton::setBone(std::string name, Transform& transform) {
		auto it = bones.find(name);
		if (it != bones.end()) {
			Transform* trans = it->second;
			trans->setMatrix(transform.getMatrix());
		}
	}

	void Skeleton::setParent(Transform& parent) {
		parent.addChild(std::unique_ptr<Transform>(rootBone));
	}

	void Skeleton::addBone(Transform* transform) {
		bones[transform->getName()] = transform;

		transform->iterateChildren([this](Transform& child) {
			addBone(&child);
		});
	}

}