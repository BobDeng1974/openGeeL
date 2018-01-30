#ifndef VIEWFRUSTUM_H
#define VIEWFRUSTUM_H

#include <vec3.hpp>
#include "objectwrapper.h"
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

		//Update frustm with transformational data
		void update(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);

		float getFOV() const;
		float getAspectRatio() const;
		float getAspectRatioInverse() const;
		float getNearPlane() const;
		float getFarPlane() const;

		void setFOV(float value);
		void setAspectRatio(float value);
		void setNearPlane(float value);
		void setFarPlane(float value);

		const Plane& getPlane(unsigned int side) const;

	private:
		AtomicWrapper<float> near, far, aspect, fov, tan;
		float nearWidth, nearHeight, farWidth, farHeight;
		Plane planes[6];

		void updateParameters();

	};

}

#endif
