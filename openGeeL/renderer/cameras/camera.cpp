#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <gtc/type_ptr.hpp>
#include <glm.hpp>
#include "shader/sceneshader.h"
#include "camera.h"
#include "inputmanager.h"
#include "transformation/transform.h"
#include "utility/rendertime.h"

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
		shader.bind<glm::vec3>(shader.cameraName + ".position", transform.getPosition());
		shader.bind<glm::mat4>("view", viewMatrix);
		shader.bind<glm::mat4>("projection", projectionMatrix);
	}

	void Camera::bindPosition(const RenderShader& shader, std::string name) const {
		shader.bind<glm::vec3>(name, transform.getPosition());
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



	SceneCamera::SceneCamera(Transform& transform, float nearClip, float farClip, const std::string& name)
		: Camera(transform, name), nearClip(nearClip), farClip(farClip) {}


	void SceneCamera::lateUpdate() {
		SceneObject::lateUpdate();

		viewMatrix = computeViewMatrix();
		inverseView = inverse(viewMatrix);

		originViewSpace = TranslateToViewSpace(glm::vec3(0.f, 0.f, 0.f));
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
		projectionMatrix = computeProjectionMatrix();

		for (auto it = callbacks.begin(); it != callbacks.end(); it++) {
			auto func = *it;

			func(nearClip, farClip);
		}
	}

	

}