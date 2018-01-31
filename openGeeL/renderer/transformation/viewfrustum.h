#ifndef VIEWFRUSTUM_H
#define VIEWFRUSTUM_H

#include <mat4x4.hpp>
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
		//Update frustm with transformational data
		virtual void update(const Transform& transform) = 0;
		virtual void update(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up) = 0;

		virtual void setParameters(const glm::mat4& projection) = 0;

		virtual float getNearPlane() const = 0;
		virtual float getFarPlane() const = 0;

		virtual void setNearPlane(float value) = 0;
		virtual void setFarPlane(float value) = 0;

		virtual const Plane& getPlane(unsigned int side) const = 0;

		bool inView(const glm::vec3& point) const;

	};
	


	class PerspectiveFrustum : public ViewFrustum {

	public:
		PerspectiveFrustum(float fov, float aspectRatio, float nearPlane, float farPlane);

		virtual void update(const Transform& transform);
		virtual void update(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);

		float getFOV() const;
		float getAspectRatio() const;
		float getAspectRatioInverse() const;

		virtual float getNearPlane() const;
		virtual float getFarPlane() const;

		virtual void setParameters(const glm::mat4& projection);
		void setParameters(float fov, float aspectRatio, float nearPlane, float farPlane);

		void setFOV(float value);
		void setAspectRatio(float value);

		virtual void setNearPlane(float value);
		virtual void setFarPlane(float value);

		virtual const Plane& getPlane(unsigned int side) const;
		

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
