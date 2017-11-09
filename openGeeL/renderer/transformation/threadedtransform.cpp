#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "appglobals.h"
#include "threadedtransform.h"

#define transformLock() std::lock_guard<std::recursive_mutex> guard(mutex);

namespace geeL {

	ThreadedTransform::ThreadedTransform(mat4& matrix, bool isStatic) 
		: Transform(matrix, isStatic) {}

	ThreadedTransform::ThreadedTransform(vec3 position, glm::quat rotation, vec3 scaling, bool isStatic) 
		: Transform(position, rotation, scaling, isStatic){}

	ThreadedTransform::ThreadedTransform(vec3 position, vec3 rotation, vec3 scaling, bool isStatic) 
		: Transform(position, rotation, scaling, isStatic) {}

	ThreadedTransform::ThreadedTransform(const Transform& transform) : Transform(transform) {}



	const glm::vec3& ThreadedTransform::getPosition() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::getPosition();
	}

	const glm::quat& ThreadedTransform::getRotation() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::getRotation();
	}

	const glm::vec3& ThreadedTransform::getScaling() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::getScaling();
	}

	const glm::vec3& ThreadedTransform::getForwardDirection() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::getForwardDirection();
	}

	const glm::vec3& ThreadedTransform::getRightDirection() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::getRightDirection();
	}

	const glm::vec3& ThreadedTransform::getUpDirection() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::getUpDirection();
	}

	const glm::mat4& ThreadedTransform::getMatrix() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::getMatrix();
	}


	void ThreadedTransform::setPosition(const vec3& position) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::setPosition(position);
	}

	void ThreadedTransform::setRotation(const glm::quat& quaternion) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::setRotation(quaternion);
	}

	void ThreadedTransform::setForward(const vec3& value) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::setForward(value);
	}

	void ThreadedTransform::setScaling(const vec3& scaling) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::setScaling(scaling);
	}

	void ThreadedTransform::setMatrix(const mat4& matrix) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::setMatrix(matrix);
	}


	vec3 ThreadedTransform::getEulerAngles() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::getEulerAngles();
	}

	void ThreadedTransform::setEulerAngles(const vec3& eulerAngles) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::setEulerAngles(eulerAngles);
	}


	void ThreadedTransform::translate(const vec3& translation) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::translate(translation);
	}

	void ThreadedTransform::rotate(const vec3& axis, float angle) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::rotate(axis, angle);
	}

	void ThreadedTransform::scale(const vec3& scalar) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::scale(scalar);
	}


	mat4 ThreadedTransform::lookAt() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		vec3 p = position;
		vec3 f = forward;
		vec3 u = up;
		
		return glm::lookAt(p, p + f, u);
	}


	void ThreadedTransform::iterateChildren(std::function<void(Transform&)> function) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::iterateChildren(function);
	}

	Transform & ThreadedTransform::AddChild(const Transform & child) {
		return AddChild(new ThreadedTransform(child));
	}


	Transform& ThreadedTransform::AddChild(Transform* child) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		ThreadedTransform* t = dynamic_cast<ThreadedTransform*>(child);
		if (t == nullptr)
			std::cout << "Warning: Adding non-threaded transform isn't " 
				<< "thread safe and may cause data races\n";
		
		return Transform::AddChild(child);
	}

	void ThreadedTransform::RemoveChild(Transform& child) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::RemoveChild(child);
	}


	Transform* ThreadedTransform::GetParent() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::GetParent();
	}

	void ThreadedTransform::ChangeParent(Transform& newParent) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::ChangeParent(newParent);
	}


	void ThreadedTransform::update() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::update();
	}

	void ThreadedTransform::addChangeListener(std::function<void(const Transform&)> listener) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::addChangeListener(listener);
	}

	bool ThreadedTransform::operator==(const Transform& b) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::operator==(b);
	}

	bool ThreadedTransform::operator!=(const Transform& b) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::operator!=(b);
	}

	Transform& ThreadedTransform::operator=(const Transform& other) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::operator=(other);
	}


	
	const std::string& ThreadedTransform::getName() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::getName();
	}

	void ThreadedTransform::setName(std::string& name) {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		Transform::setName(name);
	}

	std::string ThreadedTransform::toString() {
#if MULTI_THREADING_SUPPORT
		transformLock();
#endif

		return Transform::toString();
	}
	

}