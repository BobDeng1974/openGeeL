#include <cmath>
#include <glm.hpp>
#include "transform.h"
#include "viewfrustum.h"

#define ANG2RAD 3.14159265358979323846 / 180.0
#define PI 3.1415926535

using namespace glm;

namespace geeL {

	bool ViewFrustum::inView(const vec3& point) const {
		for (int i = 0; i < 6; i++) {
			if (getPlane(i).getDistance(point) < 0.f)
				return false;
		}

		return true;
	}



	PerspectiveFrustum::PerspectiveFrustum(float fov, float aspectRatio, float nearPlane, float farPlane) {
		setParameters(fov, aspectRatio, nearPlane, farPlane);
	}


	void PerspectiveFrustum::update(const Transform& transform) {
		updatePlanes(transform.getPosition(), 
			transform.getRightDirection(), 
			transform.getUpDirection(), 
			-transform.getForwardDirection());
	}

	void PerspectiveFrustum::update(const glm::vec3 & position, const glm::vec3 & center, const glm::vec3 & up) {
		vec3 z = normalize(position - center);
		vec3 x = normalize(cross(up, z));
		vec3 y = normalize(cross(z, x));

		updatePlanes(position, x, y, z);
	}


	void PerspectiveFrustum::updatePlanes(const vec3& position, const vec3& x, const vec3& y, const vec3& z) {
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



	float PerspectiveFrustum::getFOV() const {
		return fov;
	}

	float PerspectiveFrustum::getAspectRatio() const {
		return aspect;
	}

	float PerspectiveFrustum::getAspectRatioInverse() const {
		return nearHeight / nearWidth;
	}

	float PerspectiveFrustum::getNearPlane() const {
		return near;
	}

	float PerspectiveFrustum::getFarPlane() const {
		return far;
	}


	void PerspectiveFrustum::setParameters(const glm::mat4& projection) {

		//Read frustum parameters from new projection matrix
		fov = (float)2.0 * atan(1.0 / projection[1][1]) * 180.0 / PI;
		tan = (float)std::tan(ANG2RAD * fov * 0.5);

		aspect = projection[1][1] / projection[0][0];
		near = (2.f * projection[3][2]) / (2.f * projection[2][2] - 2.f);
		far = ((projection[2][2] - 1.f) * near) / (projection[2][2] + 1.f);

		updateParameters();
	}

	void PerspectiveFrustum::setParameters(float fov, float aspectRatio, float nearPlane, float farPlane) {
		this->fov = fov;
		tan = (float)std::tan(ANG2RAD * fov * 0.5);

		aspect = aspectRatio;
		near = nearPlane;
		far = farPlane;

		updateParameters();
	}

	void PerspectiveFrustum::setFOV(float value) {
		if (fov != value) {
			fov = value;
			tan = (float)std::tan(ANG2RAD * fov * 0.5);

			updateParameters();
		}
	}

	void PerspectiveFrustum::setAspectRatio(float value) {
		if (aspect != value) {
			aspect = value;
			updateParameters();
		}
	}

	void PerspectiveFrustum::setNearPlane(float value) {
		if (near != value) {
			near = value;
			updateParameters();
		}
	}

	void PerspectiveFrustum::setFarPlane(float value) {
		if (far != value) {
			far = value;
			updateParameters();
		}
	}

	const Plane& PerspectiveFrustum::getPlane(unsigned int side) const {
		unsigned int s = (side > 5) ? 5 : side;
		return planes[s];
	}
	
	void PerspectiveFrustum::updateParameters() {
		nearHeight = near * tan;
		nearWidth = nearHeight * aspect;

		farHeight = far * tan;
		farWidth = farHeight * aspect;
	}

}