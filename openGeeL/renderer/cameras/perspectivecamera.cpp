#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "perspectivecamera.h"
#include "../inputmanager.h"
#include "../transformation/transform.h"

using namespace glm;

namespace geeL {

	PerspectiveCamera::PerspectiveCamera(Transform& transform, 
		float fov, float width, float height, float nearClip, float farClip, std::string name)
		: Camera(transform, name), FOV(fov), currentFOV(fov), width(width), height(height), nearClip(nearClip), farClip(farClip) {}

	PerspectiveCamera::PerspectiveCamera(Transform& transform, float speed, float sensitivity,
		float fov, float width, float height, float nearClip, float farClip, std::string name)
			: Camera(transform, speed, sensitivity, name), FOV(fov), currentFOV(fov), width(width), 
				height(height), nearClip(nearClip), farClip(farClip) {
	}


	mat4 PerspectiveCamera::computeProjectionMatrix() const {
		return perspective(currentFOV, width / height, nearClip, farClip);
	}

	const float PerspectiveCamera::getNearPlane() const {
		return nearClip;
	}

	const float PerspectiveCamera::getFarPlane() const {
		return farClip;
	}

	void PerspectiveCamera::setNearPlane(float near) {
		if (near > 0.f) {
			nearClip = near;
			onViewingPlaneChange();
		}
	}

	void PerspectiveCamera::setFarPlane(float far) {
		if (far > nearClip) {
			farClip = far;
			onViewingPlaneChange();
		}
	}

	float PerspectiveCamera::getFieldOfView() const {
		return currentFOV;
	}

	void PerspectiveCamera::setFieldOfView(float fov) {
		if (fov > 1.f && fov < 170.f)
			currentFOV = fov;
	}

	void PerspectiveCamera::addViewingPlaneChangeListener(
		std::function<void(float, float)> listener) {

		callbacks.push_back(listener);
	}

	void PerspectiveCamera::removeViewingPlaneChangeListener(
		std::function<void(float, float)> listener) {

		//TODO: implement this
	}

	void  PerspectiveCamera::onViewingPlaneChange() {
		for (auto it = callbacks.begin(); it != callbacks.end(); it++) {
			auto func = *it;

			func(nearClip, farClip);
		}
	}
}