#ifndef VIEWFRUSTUM_H
#define VIEWFRUSTUM_H

#include <vec3.hpp>
#include "plane.h"

namespace geeL {

	enum class IntersectionType {
		Inside,
		Outside,
		Intersect
	};


	class ViewFrustum {

	public:
		ViewFrustum(float fov, float aspectRatio, float nearPlane, float farPlane);


		void updateFrustum(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up);

		void setFOV(float value);
		void setAspectRatio(float value);
		void setNearPlane(float value);
		void setFarPlane(float value);

	private:
		float near, far, aspect, fov, tan;
		float nearWidth, nearHeight, farWidth, farHeight;
		Plane planes[6];

		void updateParameters();

	};

}

#endif
