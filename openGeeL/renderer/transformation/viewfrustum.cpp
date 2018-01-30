#include <cmath>
#include <glm.hpp>
#include "viewfrustum.h"

#define ANG2RAD 3.14159265358979323846 / 180.0

using namespace glm;

namespace geeL {

	ViewFrustum::ViewFrustum(float fov, float aspectRatio, float nearPlane, float farPlane)
		: fov(fov)
		, aspect(aspectRatio)
		, near(nearPlane)
		, far(farPlane) {}


	void ViewFrustum::updateFrustum(const vec3& position, const vec3& direction, const vec3& up) {

		//Compute camera axis
		vec3 z = normalize(position - direction);
		vec3 x = normalize(cross(up, z));
		vec3 y = normalize(cross(z, x));

		//Compute center positions
		vec3 nc = position - z * near;
		vec3 fc = position - z * far;

		//Compute frustum planes with reference point and normal

		planes[4] = Plane(nc, -z); //Near
		planes[5] = Plane(fc,  z); //Far

		//Top
		vec3 p = nc + y * nearHeight;
		vec3 span = normalize(p - position);
		vec3 normal = cross(span, x);
		planes[0] = Plane(p, normal);

		//Bottom
		p = nc - y * nearHeight;
		span = normalize(p - position);
		normal = cross(x, span);
		planes[1] = Plane(p, normal);

		//Left
		p = nc - x * nearWidth;
		span = normalize(p - position);
		normal = cross(span, y);
		planes[1] = Plane(p, normal);

		//Right
		p = nc + x * nearWidth;
		span = normalize(p - position);
		normal = cross(y, span);
		planes[1] = Plane(p, normal);

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

	void ViewFrustum::updateParameters() {
		nearHeight = near * tan;
		nearWidth = nearHeight * aspect;

		farHeight = far * tan;
		farWidth = farHeight * aspect;
	}

}