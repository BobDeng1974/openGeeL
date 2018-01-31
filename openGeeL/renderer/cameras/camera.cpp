#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm.hpp>
#include "shader/sceneshader.h"
#include "camera.h"
#include "inputmanager.h"
#include "transformation/transform.h"
#include "utility/rendertime.h"
#include "appglobals.h"

#define cameraLock() std::lock_guard<std::mutex> guard(cameraMutex);
#define sceneCameraLock() std::lock_guard<std::mutex> guard(cameraMutex);

using namespace std;
using namespace glm;

namespace geeL {

	Camera::Camera(Transform& transform, const string& name)
		: SceneObject(transform, name) {}

	
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


	void Camera::setViewMatrix(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up) {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		viewMatrix = glm::lookAt(position, center, up);
		inverseView = inverse(viewMatrix);

		vec4 o = viewMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
		originViewSpace = glm::vec3(o.x, o.y, o.z);
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
		getFrustum().setParameters(projection);
	}



	ManualCamera::ManualCamera(Transform& transform, std::unique_ptr<ViewFrustum> frustum, const std::string& name)
		: Camera(transform, name)
		, frustum(std::move(frustum)) {}


	void ManualCamera::injectTransform() {
		if (transform.hasUpdated()) {
			setViewMatrix(transform.lookAt());
			getFrustum().update(transform);
		}
	}

	void ManualCamera::setViewMatrix(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up) {
		Camera::setViewMatrix(position, center, up);

		getFrustum().update(position, center, up);
	}

	void ManualCamera::setViewMatrix(const glm::mat4& view) {
		Camera::setViewMatrix(view);

		updateFrustum();
	}

	void ManualCamera::setProjectionMatrix(const glm::mat4& projection) {
		Camera::setProjectionMatrix(projection);

		updateFrustum();
	}

	const ViewFrustum& ManualCamera::getFrustum() const {
		return *frustum;
	}

	void ManualCamera::setFrustum(std::unique_ptr<ViewFrustum> frustum) {
		this->frustum = std::move(frustum);
	}

	ViewFrustum& ManualCamera::getFrustum() {
		return *frustum;
	}

	void ManualCamera::updateFrustum() {
		const glm::mat4& inv = getInverseViewMatrix();

		vec3 position = vec3(inv[3][0], inv[3][1], inv[3][2]);
		vec3 forward = -vec3(inv[2][0], inv[2][1], inv[2][2]);
		vec3 up = vec3(inv[1][0], inv[1][1], inv[1][2]);

		getFrustum().update(position, position + forward, up);
	}



	SceneCamera::SceneCamera(Transform& transform, const std::string& name)
		: Camera(transform, name) {}


	void SceneCamera::lateUpdate() {
		SceneObject::lateUpdate();
		computeViewMatrix();
	}

	
	void SceneCamera::computeViewMatrix() {
		if (transform.hasUpdated()) {
			setViewMatrix(transform.lookAt());
			getFrustum().update(transform);
		}
	}

	const float SceneCamera::getNearPlane() const {
		return getFrustum().getNearPlane();
	}

	const float SceneCamera::getFarPlane() const {
		return getFrustum().getFarPlane();
	}

	void SceneCamera::setNearPlane(float near) {
		if (near > 0.f) {
			getFrustum().setNearPlane(near);
			onViewingPlaneChange();
		}
	}

	void SceneCamera::setFarPlane(float far) {
		if (far > getFrustum().getNearPlane()) {
			getFrustum().setFarPlane(far);
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