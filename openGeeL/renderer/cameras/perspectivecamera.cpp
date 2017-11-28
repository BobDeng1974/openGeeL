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
			: SceneCamera(transform, nearClip, farClip, name)
			, fov(fov)
			, width(float(width))
			, height(float(height))
			, aspectRatio(float(width) / float(height)) {
	
		computeProjectionMatrix();
	}


	void PerspectiveCamera::computeProjectionMatrix()  {
		setProjectionMatrix(perspective(fov(), aspectRatio(), nearClip(), farClip()));
	}

	

	float PerspectiveCamera::getFieldOfView() const {
		return fov;
	}

	void PerspectiveCamera::setFieldOfView(float fov) {
		if (fov > 1.f && fov < 170.f) {
			this->fov = fov;
			computeProjectionMatrix();
		}
			
	}

	std::vector<vec3> PerspectiveCamera::getViewBorders(float near, float far) const {
		float invAspect = height / width;
		float tanHor = tanf(glm::radians(fov / 2.f));
		float tanVer = tanf(glm::radians((fov * invAspect) / 2.f));

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