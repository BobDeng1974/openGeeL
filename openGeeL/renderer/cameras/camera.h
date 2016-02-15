#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>

using namespace glm;

namespace geeL {

class InputManager;

class Camera {

public:
	vec3 position;
	vec3 forward;
	vec3 up;
	vec3 worldUp;
	vec3 right;

	Camera() {}

	//Defines a static camera 
	Camera(const InputManager* inputManager, vec3 position, vec3 forward, vec3 up);

	//Defines a movable camera
	Camera(const InputManager* inputManager, vec3 position, vec3 up, float yaw, float pitch, float speed, float sensitivity);

	mat4 viewMatrix() const;
	virtual mat4 projectionMatrix() const = 0;

	void update();

protected:
	const InputManager* inputManager;
	float yaw;
	float pitch;
	float speed;
	float sensitivity;

	virtual void computeKeyboardInput(float deltaTime);
	virtual void computeMouseInput();
	void updateVectors();

};


}

#endif
