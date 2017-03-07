#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "perspectivecamera.h"
#include "../inputmanager.h"
#include "../transformation/transform.h"

using namespace glm;

namespace geeL {

	PerspectiveCamera::PerspectiveCamera(Transform& transform, 
		float fov, float width, float height, float nearClip, float farClip)
		: Camera(transform), FOV(fov), currentFOV(fov), width(width), height(height), nearClip(nearClip), farClip(farClip) {}

	PerspectiveCamera::PerspectiveCamera(Transform& transform, float speed, float sensitivity,
		float fov, float width, float height, float nearClip, float farClip) 
		: Camera(transform, speed, sensitivity), FOV(fov), currentFOV(fov), width(width), height(height), nearClip(nearClip), farClip(farClip) {
	}

	mat4 PerspectiveCamera::computeProjectionMatrix() const {
		return perspective(currentFOV, width / height, nearClip, farClip);
	}

	void PerspectiveCamera::handleInput(const InputManager& input) {
		Camera::handleInput(input);

		currentFOV = FOV + input.getMouseScroll();
	}

	const float& PerspectiveCamera::getFarPlane() const {
		return farClip;
	}
}