#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "threadedtransform.h"

namespace geeL {

	ThreadedTransform::ThreadedTransform(mat4& matrix, bool isStatic) 
		: Transform(matrix, isStatic) {}

	ThreadedTransform::ThreadedTransform(vec3 position, glm::quat rotation, vec3 scaling, bool isStatic) 
		: Transform(position, rotation, scaling, isStatic){}

	ThreadedTransform::ThreadedTransform(vec3 position, vec3 rotation, vec3 scaling, bool isStatic) 
		: Transform(position, rotation, scaling, isStatic) {}



	const glm::vec3& ThreadedTransform::getPosition() {
		return mutexWrapRef<glm::vec3>([](Transform& transform) -> const glm::vec3& {
			return transform.Transform::getPosition();
		});
	}

	const glm::quat& ThreadedTransform::getRotation() {
		return mutexWrapRef<glm::quat>([](Transform& transform) -> const glm::quat& {
			return transform.Transform::getRotation();
		});
	}

	const glm::vec3& ThreadedTransform::getScaling() {
		return mutexWrapRef<glm::vec3>([](Transform& transform) -> const glm::vec3& {
			return transform.Transform::getScaling();
		});
	}

	const glm::vec3& ThreadedTransform::getForwardDirection() {
		return mutexWrapRef<glm::vec3>([](Transform& transform) -> const glm::vec3& {
			return transform.Transform::getForwardDirection();
		});
	}

	const glm::vec3& ThreadedTransform::getRightDirection() {
		return mutexWrapRef<glm::vec3>([](Transform& transform) -> const glm::vec3& {
			return transform.Transform::getRightDirection();
		});
	}

	const glm::vec3& ThreadedTransform::getUpDirection() {
		return mutexWrapRef<glm::vec3>([](Transform& transform) -> const glm::vec3& {
			return transform.Transform::getUpDirection();
		});
	}

	const glm::mat4& ThreadedTransform::getMatrix() {
		return mutexWrapRef<glm::mat4>([](Transform& transform) -> const glm::mat4& {
			return transform.Transform::getMatrix();
		});
	}


	void ThreadedTransform::setPosition(const vec3& position) {
		mutexWrap([&position](Transform& transform) {
			transform.setPosition(position);
		});
	}

	void ThreadedTransform::setRotation(const glm::quat& quaternion) {
		mutexWrap([&quaternion](Transform& transform) {
			transform.setRotation(quaternion);
		});
	}

	void ThreadedTransform::setScaling(const vec3& scaling) {
		mutexWrap([&scaling](Transform& transform) {
			transform.setScaling(scaling);
		});
	}

	void ThreadedTransform::setMatrix(const mat4& matrix) {
		mutexWrap([&matrix](Transform& transform) {
			transform.setMatrix(matrix);
		});
	}


	vec3 ThreadedTransform::getEulerAngles() {
		return mutexWrap<glm::vec3>([](Transform& transform) -> glm::vec3 {
			return transform.Transform::getEulerAngles();
		});
	}

	void ThreadedTransform::setEulerAngles(const vec3& eulerAngles) {
		mutexWrap([&eulerAngles](Transform& transform) {
			transform.setEulerAngles(eulerAngles);
		});
	}


	void ThreadedTransform::translate(const vec3& translation) {
		mutexWrap([&translation](Transform& transform) {
			transform.translate(translation);
		});
	}

	void ThreadedTransform::rotate(const vec3& axis, float angle) {
		mutexWrap([&axis, &angle](Transform& transform) {
			transform.rotate(axis, angle);
		});
	}

	void ThreadedTransform::scale(const vec3& scalar) {
		mutexWrap([&scalar](Transform& transform) {
			transform.scale(scalar);
		});
	}


	mat4 ThreadedTransform::lookAt() {
		mutex.lock();
		vec3 p = position;
		vec3 f = forward;
		vec3 u = up;
		mutex.unlock();
		
		return glm::lookAt(p, p + f, u);
	}


	void ThreadedTransform::iterateChildren(std::function<void(Transform&)> function) {
		mutexWrap([&function](Transform& transform) {
			transform.Transform::iterateChildren(function);
		});
	}


	Transform& ThreadedTransform::AddChild(Transform* child) {
		ThreadedTransform* t = dynamic_cast<ThreadedTransform*>(child);
		if (t == nullptr)
			std::cout << "Warning: Adding non-threaded transform isn't " 
				<< "thread safe and may cause data races\n";

		mutexWrap([&child](Transform& transform) {
			transform.Transform::AddChild(child);
		});
	}

	void ThreadedTransform::RemoveChild(Transform& child) {
		mutexWrap([&child](Transform& transform) {
			transform.Transform::RemoveChild(child);
		});
	}


	void ThreadedTransform::ChangeParent(Transform& newParent) {
		mutexWrap([&newParent](Transform& transform) {
			transform.Transform::ChangeParent(newParent);
		});
	}


	void ThreadedTransform::update() {
		mutexWrap([](Transform& transform) {
			transform.Transform::update();
		});
	}

	void ThreadedTransform::addChangeListener(std::function<void(const Transform&)> listener) {
		mutexWrap([&listener](Transform& transform) {
			transform.Transform::addChangeListener(listener);
		});
	}


	const std::string& ThreadedTransform::getName() {
		return mutexWrapRef<std::string>([](Transform& transform) -> const std::string& {
			return transform.Transform::getName();
		});
	}

	void ThreadedTransform::setName(std::string& name) {
		mutexWrap([&name](Transform& transform) {
			transform.Transform::setName(name);
		});
	}


	void ThreadedTransform::mutexWrap(std::function<void(Transform&)> function) {
		mutex.lock();
		function(*this);
		mutex.unlock();
	}


}