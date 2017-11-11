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
#include "appglobals.h"

#define pi 3.141592f

#define cameraLock() std::lock_guard<std::mutex> guard(cameraMutex);
#define sceneCameraLock() std::lock_guard<std::mutex> guard(cameraMutex);

using namespace std;
using namespace glm;

namespace geeL {

	Camera::Camera(Transform& transform, const string& name) : SceneObject(transform, name) {}


	
	mat4 Camera::getViewMatrix() const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		return viewMatrix;
	}

	mat4 Camera::getInverseViewMatrix() const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		return inverseView;
	}

	mat4 Camera::getProjectionMatrix() const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		return projectionMatrix;
	}


	void Camera::bindViewMatrix(const Shader& shader, ShaderLocation location) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		shader.bind<glm::mat4>(location, viewMatrix);
	}

	void Camera::bindViewMatrix(const Shader& shader, const std::string & name) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		shader.bind<glm::mat4>(name, viewMatrix);
	}

	void Camera::bindInverseViewMatrix(const Shader& shader, ShaderLocation location) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		shader.bind<glm::mat4>(location, inverseView);
	}

	void Camera::bindInverseViewMatrix(const Shader& shader, const std::string& name) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		shader.bind<glm::mat4>(name, inverseView);
	}

	void Camera::bindProjectionMatrix(const Shader& shader, ShaderLocation location) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		shader.bind<glm::mat4>(location, projectionMatrix);
	}

	void Camera::bindProjectionMatrix(const Shader& shader, const std::string & name) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		shader.bind<glm::mat4>(name, projectionMatrix);
	}

	void Camera::bind(const SceneShader& shader) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		shader.bind<glm::vec3>("camera.position", transform.getPosition());
		shader.bind<glm::mat4>("view", viewMatrix);
		shader.bind<glm::mat4>("projection", projectionMatrix);
	}

	void Camera::bindPosition(const RenderShader& shader, std::string name) const {
		shader.bind<glm::vec3>(name, transform.getPosition());
	}

	void Camera::uniformBind(int uniformID) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projectionMatrix));

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewMatrix));

		glBindBuffer(GL_UNIFORM_BUFFER, uniformID);
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3), &transform.getPosition());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	glm::vec3 Camera::TranslateToScreenSpace(const glm::vec3& vector) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		glm::vec4 vec = projectionMatrix * viewMatrix * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x / vec.w, vec.y / vec.w, vec.z / vec.w) * 0.5f + 0.5f;
	}

	glm::vec3 Camera::TranslateToViewSpace(const glm::vec3& vector) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		glm::vec4 vec = viewMatrix * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	glm::vec3 Camera::TranslateToWorldSpace(const glm::vec3& vector) const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		glm::vec4 vec = inverseView * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	const glm::vec3& Camera::GetOriginInViewSpace() const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		return originViewSpace;
	}

	bool Camera::inView(const glm::vec3& position) const {
		glm::vec3 p = TranslateToScreenSpace(position);
		return (p.x > 0.f) && (p.x < 1.f) && (p.y > 0.f) && (p.y < 1.f);
	}

	bool Camera::isBehind(const glm::vec3& position) const {
		glm::vec3 p = TranslateToScreenSpace(position);
		return p.z > 1.f;
	}

	void Camera::setViewMatrix(const glm::mat4& view) {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		viewMatrix = view;
		inverseView = inverse(viewMatrix);

		vec4 o = viewMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
		originViewSpace = glm::vec3(o.x, o.y, o.z);
	}

	void Camera::setProjectionMatrix(const glm::mat4& projection) {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		projectionMatrix = projection;
	}


	SimpleCamera::SimpleCamera(Transform& transform, const std::string& name) : Camera(transform, name) {}


	SceneCamera::SceneCamera(Transform& transform, float nearClip, float farClip, const std::string& name)
		: Camera(transform, name), nearClip(nearClip), farClip(farClip) {}


	void SceneCamera::lateUpdate() {
		SceneObject::lateUpdate();
		computeViewMatrix();
	}

	
	void SceneCamera::computeViewMatrix() {
		setViewMatrix(transform.lookAt());
	}

	void SceneCamera::updateDepth(const ScreenInfo& info) {
#if MULTI_THREADING_SUPPORT
		sceneCameraLock();
#endif

		this->info = info;
		depth = info.CTdepth;

		center = transform.getPosition() + transform.getForwardDirection() * depth;
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

	void SceneCamera::addChangeListener(std::function<void(const SceneCamera&)> listener) {
#if MULTI_THREADING_SUPPORT
		sceneCameraLock();
#endif

		callbacks.push_back(listener);
	}


	void SceneCamera::onViewingPlaneChange() {
		computeProjectionMatrix();

#if MULTI_THREADING_SUPPORT
		sceneCameraLock();
#endif

		for (auto it = callbacks.begin(); it != callbacks.end(); it++) {
			auto func = *it;

			func(*this);
		}
	}

}