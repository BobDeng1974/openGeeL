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

#define PI 3.1415926535

#define cameraLock() std::lock_guard<std::mutex> guard(cameraMutex);
#define sceneCameraLock() std::lock_guard<std::mutex> guard(cameraMutex);

using namespace std;
using namespace glm;

namespace geeL {

	Camera::Camera(Transform& transform, const ViewFrustum& frustum, const string& name)
		: SceneObject(transform, name)
		, frustum(frustum) {}

	
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

	const ViewFrustum& Camera::getFrustum() const {
#if MULTI_THREADING_SUPPORT
		cameraLock();
#endif

		return frustum;
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

		//Read frustum parameters from new projection matrix
		double fov = 2.0 * atan(1.0 / projection[1][1]) * 180.0 / PI;
		float aspect = projection[1][1] / projection[0][0];

		float near = (2.f * projection[3][2]) / (2.f * projection[2][2] - 2.f);
		float far = ((projection[2][2] - 1.f) * near) / (projection[2][2] + 1.f);

		frustum.setParameters(float(fov), aspect, near, far);
	}



	ManualCamera::ManualCamera(Transform& transform, const std::string& name)
		: Camera(transform, ViewFrustum(0.f, 0.f, 0.f, 0.f), name) {}



	SceneCamera::SceneCamera(Transform& transform, 
		const ViewFrustum& frustum,
		const std::string& name)
			: Camera(transform, frustum, name) {}


	void SceneCamera::lateUpdate() {
		SceneObject::lateUpdate();
		computeViewMatrix();
	}

	
	void SceneCamera::computeViewMatrix() {
		if (transform.hasUpdated()) {
			setViewMatrix(transform.lookAt());
			frustum.update(transform);
		}
	}

	const float SceneCamera::getNearPlane() const {
		return frustum.getNearPlane();
	}

	const float SceneCamera::getFarPlane() const {
		return frustum.getFarPlane();
	}

	void SceneCamera::setNearPlane(float near) {
		if (near > 0.f) {
			frustum.setNearPlane(near);
			onViewingPlaneChange();
		}
	}

	void SceneCamera::setFarPlane(float far) {
		if (far > frustum.getNearPlane()) {
			frustum.setFarPlane(far);
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