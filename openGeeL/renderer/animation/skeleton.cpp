#include "skeleton.h"

namespace geeL {


	Skeleton::Skeleton(Transform* root) : rootBone(root) {}


	Skeleton::~Skeleton() {
		delete rootBone;
	}



}