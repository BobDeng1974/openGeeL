#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "camera.h"

class Transform;

namespace geeL {

	class PerspectiveCamera : public Camera {

	public:		
		PerspectiveCamera(Transform& transform,
			float fov, float width, float height, float nearClip, float farClip);

		PerspectiveCamera(Transform& transform, float speed, float sensitivity,
			float fov, float width, float height, float nearClip, float farClip);

		virtual void handleInput(const InputManager& input);

		const float& getFarPlane() const;

	private:
		float FOV, currentFOV, width, height, 
			nearClip, farClip;

		virtual glm::mat4 computeProjectionMatrix() const;

	};
}

#endif
