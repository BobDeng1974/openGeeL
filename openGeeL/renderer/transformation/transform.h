#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <functional>
#include <list>
#include <string>

using glm::vec3;
using glm::mat4;

namespace geeL {

	class Transform {

	public:
		bool isStatic;

		Transform(Transform* parent = nullptr);
		Transform(mat4& matrix, Transform* parent = nullptr);
		Transform(vec3 position, vec3 rotation, vec3 scaling, Transform* parent = nullptr);
		~Transform();
		
		const glm::vec3& getPosition() const;
		const glm::vec3& getRotation() const;
		const glm::vec3& getScaling() const;
		const glm::vec3& getForwardDirection() const;
		const glm::vec3& getRightDirection() const;
		const glm::vec3& getUpDirection() const;
		const glm::mat4& getMatrix() const;

		void setPosition(const vec3& position);
		void setRotation(const vec3& rotation);
		void setScaling(const vec3& scaling);

		void translate(const vec3& translation);
		void rotate(const vec3& axis, float angle);
		void scale(const vec3& scalar);

		mat4 lookAt() const;

		std::list<Transform*>::iterator childrenStart();
		std::list<Transform*>::iterator childrenEnd();

		const Transform* GetParent() const;

		//Adds a copy of the given transform
		//as a child to this transform and 
		//returns a reference to it
		Transform& AddChild(const Transform& child);

		//Add the child to this transform. Memory of child will be 
		//managed by this transform.
		Transform& AddChild(Transform* child);

		void RemoveChild(Transform& child);
		void ChangeParent(Transform& newParent);

		void computeMatrix();

		bool Transform::operator==(const Transform& b) const;
		bool Transform::operator!=(const Transform& b) const;

		unsigned int getID() const;

		void addChangeListener(std::function<void(const Transform&)> listener);
		void removeChangeListener(std::function<void(const Transform&)> listener);

		const std::string& getName() const;
		void setName(std::string& name);

	private:
		vec3 position;
		vec3 rotation;
		vec3 scaling;
		vec3 forward;
		vec3 up;
		vec3 right;

		mat4 matrix;

		std::string name;
		unsigned int id;
		Transform* parent;
		std::list<Transform*> children;
		std::list<std::function<void(const Transform&)>> changeListener;

		void resetMatrix();

		void updateDirections();
		void onChange();

	};

}

#endif
