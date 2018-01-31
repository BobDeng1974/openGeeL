#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "perspectivecamera.h"
#include "inputmanager.h"
#include "transformation/transform.h"

using namespace glm;


namespace geeL {

	PerspectiveCamera::PerspectiveCamera(Transform& transform, 
		float fov, 
		unsigned int width, 
		unsigned int height, 
		float nearClip, 
		float farClip, 
		const std::string& name)
			: SceneCamera(transform, name)
			, frustum(PerspectiveFrustum(fov, float(width) / float(height),
				nearClip, farClip)) {
	
		computeProjectionMatrix();
	}


	void PerspectiveCamera::computeProjectionMatrix()  {
		setProjectionMatrix(perspective(frustum.getFOV(), frustum.getAspectRatio(), 
			frustum.getNearPlane(), frustum.getFarPlane()));
	}

	float PerspectiveCamera::getFieldOfView() const {
		return frustum.getFOV();
	}

	void PerspectiveCamera::setFieldOfView(float fov) {
		if (fov > 1.f && fov < 170.f) {
			frustum.setFOV(fov);
			computeProjectionMatrix();
		}
	}

	std::vector<vec3> PerspectiveCamera::getViewBorders(float near, float far) const {
		float tanHor = tanf(glm::radians(frustum.getFOV() / 2.f));
		float tanVer = tanf(glm::radians((frustum.getFOV() * frustum.getAspectRatioInverse()) / 2.f));

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

	const ViewFrustum& PerspectiveCamera::getFrustum() const {
		return frustum;
	}

	ViewFrustum& PerspectiveCamera::getFrustum() {
		return frustum;
	}

	
}