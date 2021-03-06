#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <mutex>
#include <gtc/quaternion.hpp>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include "utility/listener.h"

using glm::vec3;
using glm::mat4;

namespace geeL {

	class Shader;

	enum class TransformUpdateStatus {
		None,
		NeedsUpdate,
		HasUpdated
	};


	class Transform : public ChangeActuator<Transform> {

	public:
		const bool isStatic;

		Transform();
		Transform(mat4& matrix, bool isStatic = false);
		Transform(vec3 position, glm::quat rotation, vec3 scaling, bool isStatic = false);
		Transform(vec3 position, vec3 rotation, vec3 scaling, bool isStatic = false);
		Transform(const Transform& transform);
		Transform(const Transform& transform, bool copyChildren);
		~Transform();
		
		glm::vec3 getPosition() const;
		glm::quat getRotation() const;
		glm::vec3 getScaling() const;
		glm::vec3 getForwardDirection() const;
		glm::vec3 getRightDirection() const;
		glm::vec3 getUpDirection() const;
		glm::mat4 getMatrix() const;

		void setPosition(const vec3& position);
		void setRotation(const glm::quat& quaternion);
		void setForward(const vec3& value);
		void setScaling(const vec3& scaling);
		void setMatrix(const mat4& matrix);
		void setMatrix(const mat4&& matrix);

		vec3 getEulerAngles() const;
		void setEulerAngles(const vec3& eulerAngles);

		void translate(const vec3& translation);
		void rotate(const vec3& axis, float angle);
		void scale(const vec3& scalar);

		mat4 lookAt() const;

		void iterateChildren(std::function<void(Transform&)> function);
		void iterateChildren(std::function<void(const Transform&)> function) const;

		//Returns first child (or child of children) of given name,
		//or null pointer if it doesn't exist
		Transform* find(const std::string& name);

		//Adds a copy of the given transform
		//as a child to this transform and 
		//returns a reference to it
		Transform& addChild(const Transform& child);

		//Add the child to this transform. Memory of child will be 
		//managed by this transform.
		Transform& addChild(std::unique_ptr<Transform> child);

		//Remove child from transform. Memory of child will no longer
		//be taken care off and needs to be managed by caller
		void removeChild(Transform& child);

		Transform* getParent() const;
		void changeParent(Transform& newParent);

		//Updates transformation matrix with recent changes to position, rotation and scale.
		//Needs therefore only to be called when actual changes were made.
		void update();
		void bind(const Shader& shader, const std::string& name) const;

		//Indicates whether transform has changed since last update
		bool hasUpdated() const;
		bool operator==(const Transform& b) const;
		bool operator!=(const Transform& b) const;
		Transform& operator= (const Transform& other);

		unsigned int getID() const;

		std::string toString() const;
		std::string toStringRecursive() const;

		const std::string& getName() const;
		void setName(const std::string& name);

	protected:
		glm::quat rotation;
		vec3 position;
		vec3 scaling;
		vec3 forward;
		vec3 up;
		vec3 right;

	private:
		static unsigned int idCounter;
		unsigned int id;
		std::string name;

		mat4 matrix;
		mat4 translationMatrix;
		mat4 rotationMatrix;
		mat4 scaleMatrix;

		Transform* parent;
		std::list<Transform*> children;
		TransformUpdateStatus status;

		mutable std::recursive_mutex mutex;

		void setEulerAnglesInternal(const vec3& eulerAngles);
		void resetMatrix();
		void updateDirections();

		void changeParentInternal(Transform& newParent);
		void removeChildInternal(Transform& child);

		std::string toStringRecursive(const std::string& offset, unsigned int depth) const;

	};


}

#endif
