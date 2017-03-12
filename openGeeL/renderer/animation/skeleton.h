#ifndef SKELETON_H
#define SKELETON_H

#include <list>
#include <map>
#include <string>
#include "../transformation/transform.h"

namespace geeL {


	class SkeletonBone {

	private:
		Transform transform;
		std::list<SkeletonBone> children;
	};


	class Skeleton {




	private:
		SkeletonBone rootBone;
		std::map<std::string, SkeletonBone*> bones;

	};


}

#endif
