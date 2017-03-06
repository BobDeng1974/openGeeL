#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <list>

using glm::vec3;
using glm::mat4;

namespace geeL {

	class Transform {

	public:
		bool isStatic;

		vec3 position;
		vec3 rotation;
		vec3 scaling;
		vec3 forward;
		vec3 up;
		vec3 right;

		mat4 matrix;

		Transform(Transform* parent = nullptr);
		Transform(vec3 position, vec3 rotation, vec3 scaling, Transform* parent = nullptr);
		~Transform();
		
		void setPosition(vec3 position);
		void setRotation(vec3 rotation);
		void setScale(vec3 scaling);

		void translate(vec3 translation);
		void rotate(vec3 axis, float angle);
		void scale(vec3 scalar);

		mat4 lookAt() const;

		std::list<Transform>::iterator childrenStart();
		std::list<Transform>::iterator childrenEnd();

		const Transform* GetParent() const;

		//Adds a copy of the given transform
		//as a child to this transform and 
		//returns a reference to it
		Transform& AddChild(Transform child);

		void RemoveChild(Transform& child);
		void ChangeParent(Transform& newParent);

		void computeMatrix();

		bool Transform::operator==(const Transform& b) const;
		bool Transform::operator!=(const Transform& b) const;

		unsigned int getID() const;

	private:
		
		unsigned int id;
		Transform* parent;
		std::list<Transform> children;

		void updateDirections();
	};
}

#endif
