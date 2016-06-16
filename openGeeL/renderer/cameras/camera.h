#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <vec3.hpp>
#include <mat4x4.hpp>

using namespace std;
using glm::vec3;
using glm::mat4;

namespace geeL {

class InputManager;
class Transform;
class Shader;
class Skybox;

class Camera {

public:
	Transform& transform;

	//Defines a static camera 
	Camera(Transform& transform);

	//Defines a movable camera
	Camera(Transform& transform, float speed, float sensitivity);

	mat4 viewMatrix() const;
	virtual mat4 projectionMatrix() const = 0;

	virtual void init() {}
	virtual void update() {}
	virtual void quit() {}

	virtual void handleInput(const InputManager& input);

	void setSkybox(Skybox& skybox);
	void drawSkybox() const;
	void bindSkybox(const Shader& shader) const;

	void bind(const Shader& shader) const;

protected:
	float speed;
	float sensitivity;
	Skybox* skybox;

	virtual void computeKeyboardInput(const InputManager& input, float deltaTime);
	virtual void computeMouseInput(const InputManager& input);

};


}

#endif
