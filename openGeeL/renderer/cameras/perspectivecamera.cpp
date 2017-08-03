#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "perspectivecamera.h"
#include "inputmanager.h"
#include "transformation/transform.h"

using namespace glm;


namespace geeL {

	PerspectiveCamera::PerspectiveCamera(Transform& transform, float speed, float sensitivity,
		float fov, unsigned int width, unsigned int height, float nearClip, float farClip, std::string name)
			: SceneCamera(transform, speed, sensitivity, nearClip, farClip, name), FOV(fov), currentFOV(fov), width(float(width)), 
				height(float(height)), aspectRatio(float(width) / float(height)) {
	
		projectionMatrix = std::move(computeProjectionMatrix());
	}


	mat4 PerspectiveCamera::computeProjectionMatrix() const {
		return perspective(currentFOV, aspectRatio, nearClip, farClip);
	}

	

	float PerspectiveCamera::getFieldOfView() const {
		return currentFOV;
	}

	void PerspectiveCamera::setFieldOfView(float fov) {
		if (fov > 1.f && fov < 170.f) {
			currentFOV = fov;
			projectionMatrix = std::move(computeProjectionMatrix());
		}
			
	}

	std::vector<vec3> PerspectiveCamera::getViewBorders(float near, float far) const {
		float invAspect = height / width;
		float tanHor = tanf(glm::radians(currentFOV / 2.f));
		float tanVer = tanf(glm::radians((currentFOV * invAspect) / 2.f));

		float xNear = near * tanHor;
		float xFar = far * tanHor;
		float yNear = near * tanVer;
		float yFar = far * tanVer;

		std::vector<vec3> corners = { 
			vec3(xNear, yNear, near), 
			vec3(-xNear, yNear, near), 
			vec3(xNear, -yNear, near), 
			vec3(-xNear, -yNear, near), 
			vec3(xFar, yFar, far), 
			vec3(-xFar, yFar, far), 
			vec3(xFar, -yFar, far), 
			vec3(-xFar, -yFar, far) };

		return corners;
	}

	
}