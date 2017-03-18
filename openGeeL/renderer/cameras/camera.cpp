#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <gtc/type_ptr.hpp>
#include <glm.hpp>
#include "../shader/shader.h"
#include "camera.h"
#include "../inputmanager.h"
#include "../transformation/transform.h"
#include "../utility/gbuffer.h"
#include "../utility/rendertime.h"

#define pi 3.141592f

using namespace std;
using namespace glm;

namespace geeL {

	Camera::Camera(Transform& transform, std::string name)
		: SceneObject(transform, name), speed(0), sensitivity(0) {}

	Camera::Camera(Transform& transform, float speed, float sensitivity, std::string name)
		: SceneObject(transform, name), speed(speed), sensitivity(sensitivity) {}


	void Camera::lateUpdate() {
		SceneObject::lateUpdate();

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
			transform.translate(cameraSpeed * transform.getForwardDirection());
		if (input.getKeyHold(GLFW_KEY_S) || input.getKey(GLFW_KEY_S))
			transform.translate(-cameraSpeed * transform.getForwardDirection());
		if (input.getKeyHold(GLFW_KEY_A) || input.getKey(GLFW_KEY_A))
			transform.translate(-cameraSpeed * transform.getRightDirection());
		if (input.getKeyHold(GLFW_KEY_D) || input.getKey(GLFW_KEY_D))
			transform.translate(cameraSpeed * transform.getRightDirection());
	}


	void Camera::computeMouseInput(const InputManager& input) {

		if (input.getMouseKey(1)) {
			float yOffset = input.getMouseYOffset() * sensitivity * 0.01;
			float xOffset = -input.getMouseXOffset() * sensitivity * 0.01f;

			transform.rotate(transform.getRightDirection(), yOffset);
			transform.rotate(transform.getUpDirection(), xOffset);
		}
	}

	void Camera::bind(const SceneShader& shader) const {
		shader.use();

		shader.setVector3(shader.cameraName + ".position", transform.getPosition());
		shader.setMat4("view", viewMatrix);
		shader.setMat4("projection", projectionMatrix);
	}

	void Camera::bindPosition(const Shader& shader, std::string name) const {
		shader.use();
		shader.setVector3(name, transform.getPosition());
	}

	void Camera::updateDepth(const ScreenInfo& info) {
		this->info = &info;
		depth = info.CTdepth;

		center = transform.getPosition() + transform.getForwardDirection() * depth;
	}

	void Camera::uniformBind(int uniformID) const {

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projectionMatrix));

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewMatrix));

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3), &transform.getPosition());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	const glm::vec3& Camera::getPosition() const {
		return transform.getPosition();
	}

	const glm::vec3& Camera::getDirection() const {
		return transform.getForwardDirection();
	}

	float Camera::getSpeed() const {
		return speed;
	}

	float Camera::getSensitivity() const {
		return sensitivity;
	}

	void Camera::setSpeed(float speed) {
		if (speed > 0.f)
			this->speed = speed;
	}

	void Camera::setSensitivity(float sensitivity) {
		if (sensitivity > 0.f)
			this->sensitivity = sensitivity;
	}

}