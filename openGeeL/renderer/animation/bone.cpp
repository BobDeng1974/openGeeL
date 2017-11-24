#include "bone.h"

namespace geeL {
	
	/*
	Bone::Bone(const Bone& other) : Transform(other, false) {
	
		other.iterateChildren([this](const Transform& child) {
			const Bone* bone = dynamic_cast<const Bone*>(&child);

			if (bone != nullptr)
				this->addBone(*bone);
			else
				this->addChild(child);
		});
	
	
	}
	

	Transform& Bone::addBone(const Bone& child) {
		std::unique_ptr<Transform> newChild(new Bone(child));

		return addChild(std::move(newChild));
	}

	void Bone::iterateBones(std::function<void(Bone&)> function) {
		iterateChildren([&function](Transform& child) {
			Bone* bone = dynamic_cast<Bone*>(&child);
			if (bone != nullptr) function(*bone);
		});
	}
	*/

}