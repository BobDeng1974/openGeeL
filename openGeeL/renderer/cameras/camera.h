#ifndef CAMERA_H
#define CAMERA_H

//#include "../renderobject.h"
#include <glm.hpp>

using namespace glm;

namespace geeL {

class InputManager;
class Transform;
class Shader;

class Camera {

public:
	Transform* transform;

	Camera() {}
	~Camera();

	//Defines a static camera 
	Camera(Transform* transform);

	//Defines a movable camera
	Camera(Transform* transform, float speed, float sensitivity);

	mat4 viewMatrix() const;
	virtual mat4 projectionMatrix() const = 0;

	virtual void init() {}
	virtual void update() {}
	virtual void quit() {}

	virtual void handleInput(const InputManager& input);

	void bind(const Shader& shader) const;

protected:
	float speed;
	float sensitivity;

	virtual void computeKeyboardInput(const InputManager& input, float deltaTime);
	virtual void computeMouseInput(const InputManager& input);
};


}

#endif
