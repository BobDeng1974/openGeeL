#ifndef VIEWFRUSTUM_H
#define VIEWFRUSTUM_H

#include <vec3.hpp>
#include "objectwrapper.h"
#include "plane.h"

namespace geeL {

	class Transform;

	enum class IntersectionType {
		Inside = 1,
		Outside = 2,
		Intersect = 3
	};


	class ViewFrustum {

	public:
		ViewFrustum(float fov, float aspectRatio, float nearPlane, float farPlane);

		//Update frustm with transformational data
		void update(const Transform& transform);
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
		bool inView(const glm::vec3& point) const;

	private:
		Plane planes[6];
		AtomicWrapper<float> near, far, aspect, fov,
			nearWidth, nearHeight, farWidth, farHeight;
		float tan;

		void updateParameters();
		void updatePlanes(const glm::vec3& position, const glm::vec3& x,
			const glm::vec3& y, const glm::vec3& z);

	};

}

#endif
