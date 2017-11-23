#include "bone.h"

namespace geeL {

	unsigned int Bone::getBoneID() const {
		return boneID;
	}

	void Bone::setBoneID(unsigned int id) {
		boneID = id;
	}

}