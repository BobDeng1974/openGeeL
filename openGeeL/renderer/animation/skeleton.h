#ifndef SKELETON_H
#define SKELETON_H

#include <functional>
#include <list>
#include <map>
#include <string>
#include "bone.h"

namespace geeL {

	class Transform;

	class Skeleton {

	public:
		Skeleton();
		Skeleton(Bone& root);
		Skeleton(const Skeleton& other);
		Skeleton(Skeleton&& other);
		~Skeleton();

		Skeleton& operator=(Skeleton&& other);

		Bone* const getRootBone();
		Bone* const getBone(const std::string& name);
		const Bone* const getBone(const std::string& name) const;
		
		void iterateBones(std::function<void(const Bone&)> function) const;
		void iterateBones(std::function<void(Bone&)> function);

		void setParent(Transform& parent);

	private:
		Bone* rootBone;
		std::map<std::string, Bone*> bones;

		Skeleton& operator=(const Skeleton& other) = delete;

		void addBone(Bone* transform);

	};
}

#endif
