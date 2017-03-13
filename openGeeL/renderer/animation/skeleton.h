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

		const Transform* getBone(std::string name) const;
		void setBone(std::string name, const Transform& transform);

	private:
		Transform* rootBone;
		std::map<std::string, Transform*> bones;

		void addBone(Transform* transform);

	};

}

#endif
