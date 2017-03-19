#ifndef SKELETON_H
#define SKELETON_H

#include <list>
#include <map>
#include <string>

namespace geeL {

	class Transform;

	class Skeleton {

	public:
		//Create new skeleton with given root bone. It is assumed
		//that bone structure is already complete and no new bones
		//will be added
		Skeleton(Transform* const root);
		~Skeleton();

		unsigned int getBoneID(std::string name) const;

		Transform* const getRootBone();
		Transform* const getBone(std::string name);
		const Transform* const getBone(std::string name) const;

		void setBone(std::string name, const Transform& transform);

	private:
		Transform* rootBone;
		std::map<std::string, Transform*> bones;

		void addBone(Transform* transform);

	};
}

#endif
