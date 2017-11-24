#include <iostream>
#include "transformation/transform.h"
#include "bone.h"
#include "skeleton.h"

namespace geeL {

	Skeleton::Skeleton() : rootBone(nullptr) {}

	Skeleton::Skeleton(Bone& root) : rootBone(&root) {
		addBone(&root);
	}

	Skeleton::Skeleton(const Skeleton& other)
		: rootBone(new Bone(*other.rootBone)) {

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
		if (rootBone != nullptr && rootBone->getParent() == nullptr)
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


	Bone* const Skeleton::getRootBone() {
		return rootBone;
	}

	Bone* const Skeleton::getBone(const std::string& name) {
		auto it = bones.find(name);
		if (it != bones.end())
			return it->second;

		return nullptr;
	}

	const Bone* const Skeleton::getBone(const std::string& name) const {
		auto it = bones.find(name);
		if (it != bones.end())
			return it->second;

		return nullptr;
	}

	void Skeleton::setParent(Transform& parent) {
		parent.addChild(std::unique_ptr<Transform>(rootBone));
	}

	void Skeleton::addBone(Bone* transform) {
		bones[transform->getName()] = transform;
		
		transform->iterateChildren([this](Transform& child) {
			addBone(&child);
		});
	}

}