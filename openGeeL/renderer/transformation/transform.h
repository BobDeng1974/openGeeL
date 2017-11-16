#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <mutex>
#include <gtc/quaternion.hpp>
#include <functional>
#include <list>
#include <string>

using glm::vec3;
using glm::mat4;

namespace geeL {

	class Shader;

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
		Transform(const Transform& transform);
		~Transform();
		
		
		virtual glm::vec3 getPosition() const;
		virtual glm::quat getRotation() const;
		virtual glm::vec3 getScaling() const;
		virtual glm::vec3 getForwardDirection() const;
		virtual glm::vec3 getRightDirection() const;
		virtual glm::vec3 getUpDirection() const;
		virtual glm::mat4 getMatrix() const;

		virtual void setPosition(const vec3& position);
		virtual void setRotation(const glm::quat& quaternion);
		virtual void setForward(const vec3& value);
		virtual void setScaling(const vec3& scaling);
		virtual void setMatrix(const mat4& matrix);
		virtual void setMatrix(const mat4&& matrix);

		virtual vec3 getEulerAngles() const;
		virtual void setEulerAngles(const vec3& eulerAngles);

		virtual void translate(const vec3& translation);
		virtual void rotate(const vec3& axis, float angle);
		virtual void scale(const vec3& scalar);

		virtual mat4 lookAt() const;

		virtual void iterateChildren(std::function<void(Transform&)> function);

		//Adds a copy of the given transform
		//as a child to this transform and 
		//returns a reference to it
		virtual Transform& AddChild(const Transform& child);

		//Add the child to this transform. Memory of child will be 
		//managed by this transform.
		virtual Transform& AddChild(Transform* child);

		//Remove child from transform. Memory of child will no longer
		//be taken care off and needs to be managed by caller
		virtual void RemoveChild(Transform& child);

		virtual Transform* GetParent() const;
		virtual void ChangeParent(Transform& newParent);

		//Updates transformation matrix with recent changes to position, rotation and scale.
		//Needs therefore only to be called when actual changes were made.
		virtual void update();
		virtual void bind(const Shader& shader, const std::string& name) const;

		virtual bool operator==(const Transform& b) const;
		virtual bool operator!=(const Transform& b) const;
		virtual Transform& operator= (const Transform& other);

		virtual unsigned int getID() const;

		virtual void addChangeListener(std::function<void(const Transform&)> listener);

		virtual const std::string& getName() const;
		virtual void setName(const std::string& name);

		virtual std::string toString() const;

	protected:
		glm::quat rotation;
		vec3 position;
		vec3 scaling;
		vec3 forward;
		vec3 up;
		vec3 right;

	private:
		unsigned int id;
		std::string name;

		mat4 matrix;
		mat4 translationMatrix;
		mat4 rotationMatrix;
		mat4 scaleMatrix;

		Transform* parent;
		std::list<Transform*> children;
		std::list<std::function<void(const Transform&)>> changeListener;
		TransformUpdateStatus status;

		mutable std::mutex mutex;

		void setEulerAnglesInternal(const vec3& eulerAngles);
		void resetMatrix();
		void updateDirections();
		void onChange();

		//Returns true if the transform has changed during the 
		//current cycle (since last 'update' call)
		virtual bool hasUpdated() const;
	};


}

#endif
