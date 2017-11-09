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
		virtual Transform& AddChild(const Transform& child);
		virtual Transform& AddChild(Transform* child);
		virtual void RemoveChild(Transform& child);

		virtual Transform* GetParent() const;
		virtual void ChangeParent(Transform& newParent);

		virtual void update();
		virtual void bind(const Shader& shader, const std::string& name) const;
		virtual void addChangeListener(std::function<void(const Transform&)> listener);

		virtual bool operator==(const Transform& b) const;
		virtual bool operator!=(const Transform& b) const;
		virtual Transform& operator= (const Transform& other);

		virtual unsigned int getID() const;

		virtual const std::string& getName() const;
		virtual void setName(std::string& name);
		virtual std::string toString() const;

	private:
		mutable std::recursive_mutex mutex;

	};


}

#endif
