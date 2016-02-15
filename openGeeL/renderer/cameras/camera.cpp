#include <glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "camera.h"
#include "../inputmanager.h"

#define pi 3.141592f

namespace geeL {

	Camera::Camera(const InputManager* inputManager, vec3 position, vec3 forward, vec3 up)
		: inputManager(inputManager), position(position), forward(forward), up(up), worldUp(up), yaw(0), pitch(0), speed(0), sensitivity(0) {

	}

	Camera::Camera(const InputManager* inputManager, vec3 position, vec3 up, float yaw, float pitch, float speed, float sensitivity)
		: inputManager(inputManager), position(position), up(up), worldUp(up), yaw(yaw), pitch(pitch), speed(speed), sensitivity(sensitivity) {

		updateVectors();
	}

	mat4 Camera::viewMatrix() const {
		return glm::lookAt(position, position + forward, up);
	}

	float lastFrame = 0.f;
	void Camera::update() {
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		computeKeyboardInput(deltaTime);
		computeMouseInput();
	}


	void Camera::computeKeyboardInput(float deltaTime) {

		GLfloat cameraSpeed = speed * deltaTime;

		if (inputManager->getKeyHold(GLFW_KEY_LEFT_SHIFT) || inputManager->getKey(GLFW_KEY_LEFT_SHIFT))
			cameraSpeed *= 2.5f;
		else if (inputManager->getKeyHold(GLFW_KEY_LEFT_CONTROL) || inputManager->getKey(GLFW_KEY_LEFT_CONTROL))
			cameraSpeed *= 0.5f;

		if (inputManager->getKeyHold(GLFW_KEY_W) || inputManager->getKey(GLFW_KEY_W))
			position += cameraSpeed * forward;
		if (inputManager->getKeyHold(GLFW_KEY_S) || inputManager->getKey(GLFW_KEY_S))
			position -= cameraSpeed * forward;
		if (inputManager->getKeyHold(GLFW_KEY_A) || inputManager->getKey(GLFW_KEY_A))
			position -= glm::normalize(glm::cross(forward, up)) * cameraSpeed;
		if (inputManager->getKeyHold(GLFW_KEY_D) || inputManager->getKey(GLFW_KEY_D))
			position += glm::normalize(glm::cross(forward, up)) * cameraSpeed;
	}

	void Camera::computeMouseInput() {
		GLfloat xoffset = inputManager->getMouseXOffset() / 100.f;
		GLfloat yoffset = inputManager->getMouseYOffset() / 100.f;

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > pi - 0.1f)
			pitch = pi - 0.1f;
		if (pitch < -pi + 0.1f)
			pitch = -pi + 0.1f;

		updateVectors();
	}

	void Camera::updateVectors() {
		glm::vec3 front;
		front.x = cos(yaw) * cos(pitch);
		front.y = sin(pitch);
		front.z = sin(yaw) * cos(pitch);

		forward = normalize(front);
		right   = normalize(cross(forward, worldUp));
		up      = normalize(cross(right, forward));
	}

}