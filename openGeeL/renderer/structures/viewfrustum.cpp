#include <cmath>
#include <glm.hpp>
#include "transformation/transform.h"
#include "viewfrustum.h"

#define ANG2RAD 3.14159265358979323846 / 180.0
#define PI 3.1415926535

using namespace glm;

namespace geeL {

	const Plane& ViewFrustum::getPlane(unsigned int side) const {
		unsigned int s = (side > 5) ? 5 : side;
		return planes[s];
	}

	bool ViewFrustum::inView(const vec3& point) const {
		for (int i = 0; i < 6; i++) {
			if (getPlane(i).getDistance(point) < 0.f)
				return false;
		}

		return true;
	}



	PerspectiveFrustum::PerspectiveFrustum() 
		: fov(0.f), aspect(0.f), near(0.f), far(0.f), tan(0.f) {}

	PerspectiveFrustum::PerspectiveFrustum(float fov, float aspectRatio, float nearPlane, float farPlane) {
		setParameters(fov, aspectRatio, nearPlane, farPlane);
	}


	void PerspectiveFrustum::update(const Transform& transform) {
		updatePlanes(transform.getPosition(), 
			transform.getRightDirection(), 
			transform.getUpDirection(), 
			-transform.getForwardDirection());
	}

	void PerspectiveFrustum::update(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up) {
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

	
	
	void PerspectiveFrustum::updateParameters() {
		nearHeight = near * tan;
		nearWidth = nearHeight * aspect;

		farHeight = far * tan;
		farWidth = farHeight * aspect;
	}



	OrthographicFrustum::OrthographicFrustum()
		: l(0.f), r(0.f), b(0.f), t(0.f), n(0.f), f(0.f) {}

	OrthographicFrustum::OrthographicFrustum(float left, float right, float bottom, float top, float near, float far)
		: l(left), r(right), b(bottom), t(top), n(near), f(far) {}


	void OrthographicFrustum::update(const Transform& transform) {
		updatePlanes(transform.getPosition(),
			transform.getRightDirection(),
			transform.getUpDirection(),
			-transform.getForwardDirection());
	}

	void OrthographicFrustum::update(const vec3& position, const vec3& center, const vec3& up) {
		vec3 z = normalize(position - center);
		vec3 x = normalize(cross(up, z));
		vec3 y = normalize(cross(z, x));

		updatePlanes(position, x, y, z);
	}

	void OrthographicFrustum::setParameters(const glm::mat4& p) {
		l = -(p[3][0] / p[0][0]) - (1.f / p[0][0]);
		r = (2.f / p[0][0]) + l;

		b = -(p[3][1] / p[1][1]) - (1.f / p[1][1]);
		t = (2.f / p[1][1]) + t;

		n = (p[3][2] / p[2][2]) + (1.f / p[2][2]);
		f = (-2.f / p[2][2]) + n;
	}

	float OrthographicFrustum::getNearPlane() const {
		return n;
	}

	float OrthographicFrustum::getFarPlane() const {
		return f;
	}

	void OrthographicFrustum::setNearPlane(float value) {
		n = value;
	}

	void OrthographicFrustum::setFarPlane(float value) {
		f = value;
	}

	void OrthographicFrustum::updatePlanes(const vec3& position, const vec3& x, const vec3& y, const vec3& z) {

		//Top
		vec3 p = position + y * t();
		planes[0] = Plane(p, -y);

		//Bottom
		p = position + y * b();
		planes[0] = Plane(p, y);

		//Left
		p = position + x * l();
		planes[0] = Plane(p, -y);

		//Right
		p = position + x * r();
		planes[0] = Plane(p, -y);

		//Near
		p = position - z * n();
		planes[4] = Plane(p, -z);

		//Far
		p = position - z * f();
		planes[5] = Plane(p, z);
	}

}