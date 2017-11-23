#ifndef SKELETON_H
#define SKELETON_H

#include <list>
#include <map>
#include <string>

namespace geeL {

	class Transform;

	class Skeleton {

	public:
		Skeleton();
		Skeleton(Transform& root);
		Skeleton(const Skeleton& other);
		Skeleton(Skeleton&& other);
		~Skeleton();

		Skeleton& operator=(Skeleton&& other);

		unsigned int getBoneID(std::string name) const;

		Transform* const getRootBone();
		Transform* const getBone(std::string name);
		const Transform* const getBone(std::string name) const;

		void setBone(std::string name, Transform& transform);

		void setParent(Transform& parent);


	private:
		Transform* rootBone;
		std::map<std::string, Transform*> bones;

		Skeleton& operator=(const Skeleton& other) = delete;

		void addBone(Transform* transform);

	};
}

#endif
