#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <gtc/type_ptr.hpp>
#include <glm.hpp>
#include "../shader/shader.h"
#include "camera.h"
#include "../inputmanager.h"
#include "../transformation/transform.h"
#include <iostream>
#include "../utility/rendertime.h"

#define pi 3.141592f

using namespace std;
using namespace glm;

namespace geeL {

	Camera::Camera(Transform& transform)
		: SceneObject(transform), speed(0), sensitivity(0) {}

	Camera::Camera(Transform& transform, float speed, float sensitivity)
		: SceneObject(transform), speed(speed), sensitivity(sensitivity) {}


	void Camera::update() {
		viewMatrix = computeViewMatrix();
		inverseView = inverse(viewMatrix);
		projectionMatrix = computeProjectionMatrix();
	}

	const mat4& Camera::getViewMatrix() const {
		return viewMatrix;
	}

	const mat4& Camera::getInverseViewMatrix() const {
		return inverseView;
	}

	const mat4& Camera::getProjectionMatrix() const {
		return projectionMatrix;
	}

	mat4 Camera::computeViewMatrix() const {
		return transform.lookAt();
	}

	void Camera::handleInput(const InputManager& input) {
		computeKeyboardInput(input);
		computeMouseInput(input);
	}

	void Camera::computeKeyboardInput(const InputManager& input) {

		GLfloat cameraSpeed = speed * Time::deltaTime;

		if (input.getKeyHold(GLFW_KEY_LEFT_SHIFT) || input.getKey(GLFW_KEY_LEFT_SHIFT))
			cameraSpeed *= 2.5f;
		else if (input.getKeyHold(GLFW_KEY_LEFT_CONTROL) || input.getKey(GLFW_KEY_LEFT_CONTROL))
			cameraSpeed *= 0.5f;
		
		if (input.getKeyHold(GLFW_KEY_W) || input.getKey(GLFW_KEY_W))
			transform.translate(cameraSpeed * transform.forward);
		if (input.getKeyHold(GLFW_KEY_S) || input.getKey(GLFW_KEY_S))
			transform.translate(-cameraSpeed * transform.forward);
		if (input.getKeyHold(GLFW_KEY_A) || input.getKey(GLFW_KEY_A))
			transform.translate(-cameraSpeed * transform.right);
		if (input.getKeyHold(GLFW_KEY_D) || input.getKey(GLFW_KEY_D))
			transform.translate(cameraSpeed * transform.right);
	}

	void Camera::computeMouseInput(const InputManager& input) {

		if (input.getMouseKey(GLFW_MOUSE_BUTTON_2)) {
			GLfloat xoffset = input.getMouseXOffset() * sensitivity;
			GLfloat yoffset = input.getMouseYOffset() * sensitivity;

			transform.rotate(vec3(0.f, 0.f, -1.f), xoffset);
			transform.rotate(vec3(1.f, 0.f, 0.f), yoffset);
		}
	}

	void Camera::bind(const Shader& shader) const {
		shader.use();

		shader.setVector3(shader.cam + ".position", transform.position);
		shader.setMat4("view", viewMatrix);
		shader.setMat4("projection", projectionMatrix);
	}

	void Camera::bindPosition(const Shader& shader, std::string name) const {
		shader.use();
		shader.setVector3(name, transform.position);
	}

	void Camera::updateDepth(float depth) {
		this->depth = depth;

		center = transform.position + transform.forward * depth;
	}

	void Camera::uniformBind(int uniformID) const {

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projectionMatrix));
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewMatrix));
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3), &transform.position);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	const glm::vec3& Camera::getPosition() const {
		return transform.position;
	}

	const glm::vec3& Camera::getDirection() const {
		return transform.forward;
	}

}