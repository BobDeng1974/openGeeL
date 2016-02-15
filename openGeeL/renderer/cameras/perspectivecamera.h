#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include <glm.hpp>
#include "camera.h"

using namespace glm;

namespace geeL {

class PerspectiveCamera : public Camera {

public:		
	PerspectiveCamera() {};

	PerspectiveCamera(const InputManager* inputManager, vec3 position, vec3 forward, vec3 up, 
		float fov, float width, float height, float nearClip, float farClip);

	PerspectiveCamera(const InputManager* inputManager, vec3 position, vec3 up, float yaw, float pitch, float speed, float sensitivity,
		float fov, float width, float height, float nearClip, float farClip);

	virtual mat4 projectionMatrix() const;

private:
	float FOV, width, height, 
		nearClip, farClip;

	float getCurrentZoom() const;

};


}

#endif
