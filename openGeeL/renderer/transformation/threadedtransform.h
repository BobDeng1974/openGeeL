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


		virtual const glm::vec3& getPosition();
		virtual const glm::quat& getRotation();
		virtual const glm::vec3& getScaling();
		virtual const glm::vec3& getForwardDirection();
		virtual const glm::vec3& getRightDirection();
		virtual const glm::vec3& getUpDirection();
		virtual const glm::mat4& getMatrix();

		virtual void setPosition(const vec3& position);
		virtual void setRotation(const glm::quat& quaternion);
		virtual void setScaling(const vec3& scaling);
		virtual void setMatrix(const mat4& matrix);

		virtual vec3 getEulerAngles();
		virtual void setEulerAngles(const vec3& eulerAngles);

		virtual void translate(const vec3& translation);
		virtual void rotate(const vec3& axis, float angle);
		virtual void scale(const vec3& scalar);

		virtual mat4 lookAt();

		virtual void iterateChildren(std::function<void(Transform&)> function);
		virtual Transform& AddChild(Transform* child);
		virtual void RemoveChild(Transform& child);

		virtual void ChangeParent(Transform& newParent);

		virtual void update();
		virtual void addChangeListener(std::function<void(const Transform&)> listener);

		virtual const std::string& getName();
		virtual void setName(std::string& name);

	private:
		std::recursive_mutex mutex;

		void mutexWrap(std::function<void(Transform&)> function);

		template<typename T>
		T mutexWrap(std::function<T(Transform&)> function);

		template<typename T>
		const T& mutexWrapRef(std::function<T(Transform&)> function);


	};


	template<typename T>
	inline T ThreadedTransform::mutexWrap(std::function<T(Transform&)> function) {
		mutex.lock();
		T t = function(*this);
		mutex.unlock();

		return t;
	}

	template<typename T>
	inline const T& ThreadedTransform::mutexWrapRef(std::function<T(Transform&)> function) {
		//Note: This is probably not thread safe

		mutex.lock();
		const T& t = function(*this);
		mutex.unlock();

		return t;
	}



}


#endif
