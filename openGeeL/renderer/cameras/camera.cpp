#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "camera.h"
#include "../inputmanager.h"
#include "../transform.h"
#include "../shader/shader.h"
#include <iostream>

#define pi 3.141592f

namespace geeL {

	Camera::Camera(Transform* transform)
		: transform(transform), speed(0), sensitivity(0) {}

	Camera::Camera(Transform* transform, float speed, float sensitivity) 
		: transform(transform), speed(speed), sensitivity(sensitivity) {}

	Camera::~Camera() {
		delete transform;
	}

	mat4 Camera::viewMatrix() const {
		return transform->lookAt();
	}

	float lastFrame = 0.f;
	void Camera::handleInput(const InputManager& input) {
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		computeKeyboardInput(input, deltaTime);
		computeMouseInput(input);
	}

	void Camera::computeKeyboardInput(const InputManager& input, float deltaTime) {

		GLfloat cameraSpeed = speed * deltaTime;

		if (input.getKeyHold(GLFW_KEY_LEFT_SHIFT) || input.getKey(GLFW_KEY_LEFT_SHIFT))
			cameraSpeed *= 2.5f;
		else if (input.getKeyHold(GLFW_KEY_LEFT_CONTROL) || input.getKey(GLFW_KEY_LEFT_CONTROL))
			cameraSpeed *= 0.5f;
		
		if (input.getKeyHold(GLFW_KEY_W) || input.getKey(GLFW_KEY_W))
			transform->translate(cameraSpeed * transform->forward);
		if (input.getKeyHold(GLFW_KEY_S) || input.getKey(GLFW_KEY_S))
			transform->translate(-cameraSpeed * transform->forward);
		if (input.getKeyHold(GLFW_KEY_A) || input.getKey(GLFW_KEY_A))
			transform->translate(-cameraSpeed * transform->right);
		if (input.getKeyHold(GLFW_KEY_D) || input.getKey(GLFW_KEY_D))
			transform->translate(cameraSpeed * transform->right);
	}

	void Camera::computeMouseInput(const InputManager& input) {

		if (input.getKeyHold(GLFW_KEY_LEFT_ALT) || input.getKey(GLFW_KEY_LEFT_ALT)) {

			GLfloat xoffset = input.getMouseXOffset();
			GLfloat yoffset = input.getMouseYOffset();

			xoffset *= sensitivity;
			yoffset *= sensitivity;

			vec3 rotation = transform->rotation;

			//std::cout << rotation.x << ", " << rotation.y << ", " << rotation.z << "\n";

			/*
			yaw += xoffset;
			pitch += yoffset;

			if (pitch > pi - 0.1f)
			pitch = pi - 0.1f;
			if (pitch < -pi + 0.1f)
			pitch = -pi + 0.1f;

			updateVectors();
			*/

			float newPitch = transform->rotation.x + yoffset;

			if (newPitch >= 90.f)
				yoffset = 90.f - transform->rotation.x;

			//std::cout << yoffset << "\n";

			//if (newPitch < 90.f && newPitch > -90.f) {
			transform->rotate(vec3(0.f, 1.f, 0.f), xoffset);
			transform->rotate(vec3(1.f, 0.f, 0.f), yoffset);

			//}



		}
		

	}

	void Camera::bind(const Shader& shader) const {
		shader.use();

		GLint viewLoc = glGetUniformLocation(shader.program, "view");
		GLint projLoc = glGetUniformLocation(shader.program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix()));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix()));
	}
}