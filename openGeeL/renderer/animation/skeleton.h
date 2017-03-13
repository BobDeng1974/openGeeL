#ifndef SKELETON_H
#define SKELETON_H

#include <list>
#include <map>
#include <string>
#include "../transformation/transform.h"

namespace geeL {

	class Skeleton {

	public:
		Skeleton(Transform* root);
		~Skeleton();


	private:
		Transform* rootBone;
		//std::map<std::string, SkeletonBone*> bones;

	};


}

#endif
