#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "../shader/shader.h"
#include "../cubemapping/skybox.h"
#include "camera.h"
#include "../inputmanager.h"
#include "../transformation/transform.h"
#include <iostream>
#include "../utility/rendertime.h"

#define pi 3.141592f

namespace geeL {

	Camera::Camera(Transform& transform)
		: transform(transform), speed(0), sensitivity(0), skybox(nullptr) {}

	Camera::Camera(Transform& transform, float speed, float sensitivity) 
		: transform(transform), speed(speed), sensitivity(sensitivity), skybox(nullptr) {}


	mat4 Camera::viewMatrix() const {
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

		vec3 position = transform.position;
		string posName = shader.cam + ".position";
		glUniform3f(glGetUniformLocation(shader.program, posName.c_str()), position.x, position.y, position.z);
		
		GLint viewLoc = glGetUniformLocation(shader.program, "view");
		GLint projLoc = glGetUniformLocation(shader.program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix()));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix()));
	}

	void Camera::uniformBind(int uniformID) const {

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projectionMatrix()));
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewMatrix()));
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3), &transform.position);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void Camera::setSkybox(Skybox& skybox) {
		this->skybox = &skybox;
	}

	void Camera::drawSkybox() const {
		if (skybox != nullptr)
			skybox->draw(*this);
	}

	void Camera::bindSkybox(const Shader& shader) const {
		if (skybox != nullptr)
			skybox->bind(shader);
	}

}