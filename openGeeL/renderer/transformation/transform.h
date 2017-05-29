#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <gtc/quaternion.hpp>
#include <functional>
#include <list>
#include <string>

using glm::vec3;
using glm::mat4;

namespace geeL {

	enum class TransformUpdateStatus {
		None,
		NeedsUpdate,
		HasUpdated
	};


	class Transform {

	public:
		const bool isStatic;

		Transform();
		Transform(mat4& matrix, bool isStatic = false);
		Transform(vec3 position, glm::quat rotation, vec3 scaling, bool isStatic = false);
		Transform(vec3 position, vec3 rotation, vec3 scaling, bool isStatic = false);
		~Transform();
		
		const glm::vec3& getPosition() const;
		const glm::quat& getRotation() const;
		const glm::vec3& getScaling() const;
		const glm::vec3& getForwardDirection() const;
		const glm::vec3& getRightDirection() const;
		const glm::vec3& getUpDirection() const;
		const glm::mat4& getMatrix() const;

		void setPosition(const vec3& position);
		void setRotation(const glm::quat& quaternion);
		void setScaling(const vec3& scaling);
		void setMatrix(const mat4& matrix);

		vec3 getEulerAngles() const;
		void setEulerAngles(const vec3& eulerAngles);

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

		//Remove child from transform. Memory of child will no longer
		//be taken care off and needs to be managed by caller
		void RemoveChild(Transform& child);

		void ChangeParent(Transform& newParent);

		//Updates transformation matrix with recent changes to position, rotation and scale.
		//Needs therefore only to be called when actual changes were made.
		void update();

		//Returns true if the transform has changed during the 
		//current cycle (since last 'update' call)
		bool hasUpdated() const;

		bool Transform::operator==(const Transform& b) const;
		bool Transform::operator!=(const Transform& b) const;

		unsigned int getID() const;

		void addChangeListener(std::function<void(const Transform&)> listener);
		void removeChangeListener(std::function<void(const Transform&)> listener);

		const std::string& getName() const;
		void setName(std::string& name);

		virtual std::string toString() const;

	private:
		unsigned int id;
		std::string name;

		glm::quat rotation;
		vec3 position;
		vec3 scaling;
		vec3 forward;
		vec3 up;
		vec3 right;

		mat4 matrix;
		mat4 translationMatrix;
		mat4 rotationMatrix;
		mat4 scaleMatrix;

		Transform* parent;
		std::list<Transform*> children;
		std::list<std::function<void(const Transform&)>> changeListener;
		TransformUpdateStatus status;

		void setEulerAnglesInternal(const vec3& eulerAngles);

		void resetMatrix();
		void updateDirections();
		void onChange();
	};

}

#endif
