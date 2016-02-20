#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "camera.h"

using namespace glm;

class Transform;

namespace geeL {

class PerspectiveCamera : public Camera {

public:		
	PerspectiveCamera() {};

	PerspectiveCamera(Transform* transform,
		float fov, float width, float height, float nearClip, float farClip);

	PerspectiveCamera(Transform* transform, float speed, float sensitivity,
		float fov, float width, float height, float nearClip, float farClip);

	virtual mat4 projectionMatrix() const;

	virtual void handleInput(const InputManager& input);

private:
	float FOV, currentFOV, width, height, 
		nearClip, farClip;

};


}

#endif
