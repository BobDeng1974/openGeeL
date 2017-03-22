#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include <functional>
#include <list>
#include "camera.h"

namespace geeL {

	class Transform;

	class PerspectiveCamera : public Camera {

	public:		
		PerspectiveCamera(Transform& transform, float fov, unsigned int width, unsigned int height,
			float nearClip, float farClip, std::string name = "PerspectiveCamera");

		PerspectiveCamera(Transform& transform, float speed, float sensitivity, float fov, unsigned int width,
			unsigned int height, float nearClip, float farClip, std::string name = "PerspectiveCamera");

		const float getNearPlane() const;
		const float getFarPlane() const;

		void setNearPlane(float near);
		void setFarPlane(float far);

		float getFieldOfView() const;
		void setFieldOfView(float fov);

		void addViewingPlaneChangeListener(std::function<void(float, float)> listener);
		void removeViewingPlaneChangeListener(std::function<void(float, float)> listener);

	private:
		std::list<std::function<void(float, float)>> callbacks;
		float FOV, currentFOV, aspectRatio, 
			nearClip, farClip;

		virtual glm::mat4 computeProjectionMatrix() const;

		void onViewingPlaneChange();

	};
}

#endif
