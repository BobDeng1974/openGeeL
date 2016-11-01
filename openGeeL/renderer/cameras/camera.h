#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "../sceneobject.h"

using namespace std;
using glm::vec3;
using glm::mat4;

namespace geeL {

class InputManager;
class Transform;
class Shader;

class Camera : public SceneObject {

public:
	//Defines a static camera 
	Camera(Transform& transform);

	//Defines a movable camera
	Camera(Transform& transform, float speed, float sensitivity);

	//Update view and projection matrices
	void update();

	virtual void handleInput(const InputManager& input);

	void bind(const Shader& shader) const;
	void bindPosition(const Shader& shader, std::string name = "cameraPosition") const;
	void uniformBind(int uniformID) const;

	const mat4& getViewMatrix() const;
	const mat4& getProjectionMatrix() const;

protected:
	float speed;
	float sensitivity;
	mat4 viewMatrix;
	mat4 projectionMatrix;

	virtual void computeKeyboardInput(const InputManager& input);
	virtual void computeMouseInput(const InputManager& input);

	mat4 computeViewMatrix() const;
	virtual mat4 computeProjectionMatrix() const = 0;

};


}

#endif
