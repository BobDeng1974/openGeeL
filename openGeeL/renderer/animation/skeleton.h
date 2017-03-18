#ifndef SKELETON_H
#define SKELETON_H

#include <list>
#include <map>
#include <string>

namespace geeL {

	class Transform;

	class Skeleton {

	public:
		Skeleton(Transform* const root);
		~Skeleton();

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
