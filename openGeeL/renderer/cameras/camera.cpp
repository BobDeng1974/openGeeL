#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <gtc/type_ptr.hpp>
#include <glm.hpp>
#include "../shader/sceneshader.h"
#include "camera.h"
#include "../inputmanager.h"
#include "../transformation/transform.h"
#include "../utility/rendertime.h"

#define pi 3.141592f

using namespace std;
using namespace glm;

namespace geeL {

	Camera::Camera(Transform& transform, const string& name) : SceneObject(transform, name) {}


	const mat4& Camera::getViewMatrix() const {
		return viewMatrix;
	}

	const mat4& Camera::getInverseViewMatrix() const {
		return inverseView;
	}

	const mat4& Camera::getProjectionMatrix() const {
		return projectionMatrix;
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

	void Camera::uniformBind(int uniformID) const {

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projectionMatrix));

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewMatrix));

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3), &transform.getPosition());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	glm::vec3 Camera::TranslateToScreenSpace(const glm::vec3& vector) const {
		glm::vec4 vec = getProjectionMatrix() * getViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x / vec.w, vec.y / vec.w, vec.z / vec.w) * 0.5f + 0.5f;
	}

	glm::vec3 Camera::TranslateToViewSpace(const glm::vec3& vector) const {
		glm::vec4 vec = getViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	glm::vec3 Camera::TranslateToWorldSpace(const glm::vec3& vector) const {
		glm::vec4 vec = getInverseViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	const glm::vec3& Camera::GetOriginInViewSpace() const {
		return originViewSpace;
	}


	SimpleCamera::SimpleCamera(Transform& transform, const std::string& name) : Camera(transform, name) {}

	void SimpleCamera::setViewMatrix(const glm::mat4& view) {
		viewMatrix = view;
		inverseView = inverse(viewMatrix);
		originViewSpace = TranslateToViewSpace(glm::vec3(0.f, 0.f, 0.f));
	}

	void SimpleCamera::setProjectionMatrix(const glm::mat4& projection) {
		projectionMatrix = projection;
	}



	SceneCamera::SceneCamera(Transform& transform, float speed, float sensitivity, 
		float nearClip, float farClip, const std::string& name)
			: Camera(transform, name), speed(speed), sensitivity(sensitivity), 
				nearClip(nearClip), farClip(farClip) {}


	void SceneCamera::lateUpdate() {
		SceneObject::lateUpdate();

		viewMatrix = computeViewMatrix();
		inverseView = inverse(viewMatrix);
		projectionMatrix = computeProjectionMatrix();

		originViewSpace = TranslateToViewSpace(glm::vec3(0.f, 0.f, 0.f));
	}

	

	void SceneCamera::handleInput(const InputManager& input) {
		computeKeyboardInput(input);
		computeMouseInput(input);
	}

	void SceneCamera::computeKeyboardInput(const InputManager& input) {

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


	void SceneCamera::computeMouseInput(const InputManager& input) {

		if (input.getMouseKey(1)) {
			float yOffset = float(input.getMouseYOffset()) * sensitivity * 0.01f;
			float xOffset = -float(input.getMouseXOffset()) * sensitivity * 0.01f;

			transform.rotate(transform.getRightDirection(), yOffset);
			transform.rotate(transform.getUpDirection(), xOffset);
		}
	}

	mat4 SceneCamera::computeViewMatrix() const {
		return transform.lookAt();
	}

	void SceneCamera::updateDepth(const ScreenInfo& info) {
		this->info = info;
		depth = info.CTdepth;

		center = transform.getPosition() + transform.getForwardDirection() * depth;
	}

	const glm::vec3& SceneCamera::getPosition() const {
		return transform.getPosition();
	}

	const glm::vec3& SceneCamera::getDirection() const {
		return transform.getForwardDirection();
	}

	float SceneCamera::getSpeed() const {
		return speed;
	}

	float SceneCamera::getSensitivity() const {
		return sensitivity;
	}

	void SceneCamera::setSpeed(float speed) {
		if (speed > 0.f)
			this->speed = speed;
	}

	void SceneCamera::setSensitivity(float sensitivity) {
		if (sensitivity > 0.f)
			this->sensitivity = sensitivity;
	}

	const float SceneCamera::getNearPlane() const {
		return nearClip;
	}

	const float SceneCamera::getFarPlane() const {
		return farClip;
	}

	void SceneCamera::setNearPlane(float near) {
		if (near > 0.f) {
			nearClip = near;
			onViewingPlaneChange();
		}
	}

	void SceneCamera::setFarPlane(float far) {
		if (far > nearClip) {
			farClip = far;
			onViewingPlaneChange();
		}
	}

	void SceneCamera::addViewingPlaneChangeListener(
		std::function<void(float, float)> listener) {

		callbacks.push_back(listener);
	}

	void SceneCamera::removeViewingPlaneChangeListener(
		std::function<void(float, float)> listener) {

		//TODO: implement this
	}

	void  SceneCamera::onViewingPlaneChange() {
		for (auto it = callbacks.begin(); it != callbacks.end(); it++) {
			auto func = *it;

			func(nearClip, farClip);
		}
	}

	

}