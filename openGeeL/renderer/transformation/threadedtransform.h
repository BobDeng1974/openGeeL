#ifndef THREADEDTRANSFORM_H
#define THREADEDTRANSFORM_H



#include <functional>
#include <mutex>
#include "transform.h"

namespace geeL {


	class ThreadedTransform : public Transform {

	public:
		ThreadedTransform(mat4& matrix, bool isStatic = false);
		ThreadedTransform(vec3 position, glm::quat rotation, vec3 scaling, bool isStatic = false);
		ThreadedTransform(vec3 position, vec3 rotation, vec3 scaling, bool isStatic = false);
		ThreadedTransform(const Transform& transform);

		virtual const glm::vec3& getPosition();
		virtual const glm::quat& getRotation();
		virtual const glm::vec3& getScaling();
		virtual const glm::vec3& getForwardDirection();
		virtual const glm::vec3& getRightDirection();
		virtual const glm::vec3& getUpDirection();
		virtual const glm::mat4& getMatrix();

		virtual void setPosition(const vec3& position);
		virtual void setRotation(const glm::quat& quaternion);
		virtual void setForward(const vec3& value);
		virtual void setScaling(const vec3& scaling);
		virtual void setMatrix(const mat4& matrix);

		virtual vec3 getEulerAngles();
		virtual void setEulerAngles(const vec3& eulerAngles);

		virtual void translate(const vec3& translation);
		virtual void rotate(const vec3& axis, float angle);
		virtual void scale(const vec3& scalar);

		virtual mat4 lookAt();

		virtual void iterateChildren(std::function<void(Transform&)> function);
		virtual Transform& AddChild(const Transform& child);
		virtual Transform& AddChild(Transform* child);
		virtual void RemoveChild(Transform& child);

		virtual Transform* GetParent();
		virtual void ChangeParent(Transform& newParent);

		virtual void update();
		virtual void addChangeListener(std::function<void(const Transform&)> listener);

		virtual bool operator==(const Transform& b);
		virtual bool operator!=(const Transform& b);
		virtual Transform& operator= (const Transform& other);

		virtual const std::string& getName();
		virtual void setName(std::string& name);
		virtual std::string toString();

	private:
		std::recursive_mutex mutex;

	};


}

#endif
