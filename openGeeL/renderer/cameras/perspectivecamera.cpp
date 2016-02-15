#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "perspectivecamera.h"
#include "../inputmanager.h"

namespace geeL {

	PerspectiveCamera::PerspectiveCamera(const InputManager* inputManager, vec3 position, vec3 forward, vec3 up, 
		float fov, float width, float height, float nearClip, float farClip)
		: Camera(inputManager, position, forward, up), FOV(fov), width(width), height(height), nearClip(nearClip), farClip(farClip) {}

	PerspectiveCamera::PerspectiveCamera(const InputManager* inputManager, vec3 position, vec3 up, float yaw, float pitch, float speed, float sensitivity, 
		float fov, float width, float height, float nearClip, float farClip)
		: Camera(inputManager, position, up, yaw, pitch, speed, sensitivity), FOV(fov), width(width), height(height), nearClip(nearClip), farClip(farClip) {}

	mat4 PerspectiveCamera::projectionMatrix() const {
		return glm::perspective(getCurrentZoom(), width / height, nearClip, farClip);
	}

	float PerspectiveCamera::getCurrentZoom() const {
		return FOV + inputManager->getMouseScroll();
	}

}