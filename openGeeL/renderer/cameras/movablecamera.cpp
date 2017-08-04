#include <glfw3.h>
#include "utility/rendertime.h"
#include "transformation/transform.h"
#include "movablecamera.h"

namespace geeL {

	MovableCamera::MovableCamera(float speed, float sensitivity) 
		: speed(speed), sensitivity(sensitivity) {}


	void MovableCamera::update(Input& input) {
		computeKeyboardInput(input);
		computeMouseInput(input);
	}


	void MovableCamera::computeKeyboardInput(const Input& input) {
		Transform& transform = sceneObject->transform;
		float cameraSpeed = speed * RenderTime::deltaTime();

		if (input.getKeyHold(GLFW_KEY_LEFT_SHIFT) || input.getKey(GLFW_KEY_LEFT_SHIFT))
			cameraSpeed *= 2.5f;
		else if (input.getKeyHold(GLFW_KEY_LEFT_CONTROL) || input.getKey(GLFW_KEY_LEFT_CONTROL))
			cameraSpeed *= 0.5f;

		if (input.getKeyHold(GLFW_KEY_W) || input.getKey(GLFW_KEY_W))
			transform.translate(cameraSpeed * transform.getForwardDirection());
		if (input.getKeyHold(GLFW_KEY_S) || input.getKey(GLFW_KEY_S))
			transform.translate(-cameraSpeed * transform.getForwardDirection());
		if (input.getKeyHold(GLFW_KEY_A) || input.getKey(GLFW_KEY_A))
			transform.translate(-cameraSpeed * transform.getRightDirection());
		if (input.getKeyHold(GLFW_KEY_D) || input.getKey(GLFW_KEY_D))
			transform.translate(cameraSpeed * transform.getRightDirection());
	}


	void MovableCamera::computeMouseInput(const Input& input) {
		if (input.getMouseKey(1)) {
			Transform& transform = sceneObject->transform;
			float yOffset = float(input.getMouseYOffset()) * sensitivity * 0.3f * RenderTime::deltaTime();
			float xOffset = -float(input.getMouseXOffset()) * sensitivity * 0.3f * RenderTime::deltaTime();

			transform.rotate(transform.getRightDirection(), yOffset);
			transform.rotate(transform.getUpDirection(), xOffset);
		}
	}


	float MovableCamera::getSpeed() const {
		return speed;
	}

	float MovableCamera::getSensitivity() const {
		return sensitivity;
	}

	void MovableCamera::setSpeed(float speed) {
		if (speed > 0.f)
			this->speed = speed;
	}

	void MovableCamera::setSensitivity(float sensitivity) {
		if (sensitivity > 0.f)
			this->sensitivity = sensitivity;
	}


}