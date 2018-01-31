#include <cmath>
#include <glm.hpp>
#include "transform.h"
#include "viewfrustum.h"

#define ANG2RAD 3.14159265358979323846 / 180.0

using namespace glm;

namespace geeL {

	ViewFrustum::ViewFrustum(float fov, float aspectRatio, float nearPlane, float farPlane)
		: aspect(aspectRatio)
		, near(nearPlane)
		, far(farPlane) {
	
		setFOV(fov);
	}


	void ViewFrustum::update(const Transform& transform) {
		updatePlanes(transform.getPosition(), 
			transform.getRightDirection(), 
			transform.getUpDirection(), 
			-transform.getForwardDirection());
	}

	void geeL::ViewFrustum::update(const glm::vec3 & position, const glm::vec3 & center, const glm::vec3 & up) {
		vec3 z = normalize(position - center);
		vec3 x = normalize(cross(up, z));
		vec3 y = normalize(cross(z, x));

		updatePlanes(position, x, y, z);
	}


	void geeL::ViewFrustum::updatePlanes(const vec3& position, const vec3& x, const vec3& y, const vec3& z) {
		//Compute center positions
		vec3 nc = position - z * near();
		vec3 fc = position - z * far();

		//Compute frustum planes with reference point and normal

		planes[4] = Plane(nc, -z); //Near
		planes[5] = Plane(fc,  z); //Far

		//Top
		vec3 p = nc + y * nearHeight();
		vec3 span = normalize(p - position);
		vec3 normal = cross(span, x);
		planes[0] = Plane(p, normal);

		//Bottom
		p = nc - y * nearHeight();
		span = normalize(p - position);
		normal = cross(x, span);
		planes[1] = Plane(p, normal);

		//Left
		p = nc - x * nearWidth();
		span = normalize(p - position);
		normal = cross(span, y);
		planes[1] = Plane(p, normal);

		//Right
		p = nc + x * nearWidth();
		span = normalize(p - position);
		normal = cross(y, span);
		planes[1] = Plane(p, normal);
	}



	float ViewFrustum::getFOV() const {
		return fov;
	}

	float ViewFrustum::getAspectRatio() const {
		return aspect;
	}

	float ViewFrustum::getAspectRatioInverse() const {
		return nearHeight / nearWidth;
	}

	float ViewFrustum::getNearPlane() const {
		return near;
	}

	float ViewFrustum::getFarPlane() const {
		return far;
	}


	void ViewFrustum::setFOV(float value) {
		if (fov != value) {
			fov = value;
			tan = (float)std::tan(ANG2RAD * fov * 0.5);

			updateParameters();
		}
	}

	void ViewFrustum::setAspectRatio(float value) {
		if (aspect != value) {
			aspect = value;
			updateParameters();
		}
	}

	void ViewFrustum::setNearPlane(float value) {
		if (near != value) {
			near = value;
			updateParameters();
		}
	}

	void ViewFrustum::setFarPlane(float value) {
		if (far != value) {
			far = value;
			updateParameters();
		}
	}

	const Plane& ViewFrustum::getPlane(unsigned int side) const {
		unsigned int s = (side > 5) ? 5 : side;
		return planes[s];
	}

	bool ViewFrustum::inView(const vec3& point) const {
		for (int i = 0; i < 6; i++) {
			if (planes[i].getDistance(point) < 0.f)
				return false;
		}

		return true;
	}

	
	void ViewFrustum::updateParameters() {
		nearHeight = near * tan;
		nearWidth = nearHeight * aspect;

		farHeight = far * tan;
		farWidth = farHeight * aspect;
	}

}