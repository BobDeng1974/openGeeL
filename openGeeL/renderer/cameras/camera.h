#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "../sceneobject.h"


namespace geeL {

	struct ScreenInfo;
	class InputManager;
	class Transform;
	class Shader;
	class SceneShader;

	class Camera : public SceneObject {

	public:
		float depth;
		const ScreenInfo* info;
		glm::vec3 center;

		//Defines a static camera 
		Camera(Transform& transform, std::string name = "Camera");

		//Defines a movable camera
		Camera(Transform& transform, float speed, float sensitivity, std::string name = "Camera");

		//Update view and projection matrices
		virtual void lateUpdate();

		virtual void handleInput(const InputManager& input);

		void bind(const SceneShader& shader) const;
		void bindPosition(const Shader& shader, std::string name = "cameraPosition") const;
		void uniformBind(int uniformID) const;

		//Update position and depth of center pixel of camera view
		void updateDepth(const ScreenInfo& info);

		const glm::mat4& getViewMatrix() const;
		const glm::mat4& getInverseViewMatrix() const;
		const glm::mat4& getProjectionMatrix() const;

		const glm::vec3& getPosition() const;
		const glm::vec3& getDirection() const;

		float getSpeed() const;
		float getSensitivity() const;

		void setSpeed(float speed);
		void setSensitivity(float sensitivity);

	protected:
		float speed;
		float sensitivity;
		glm::mat4 viewMatrix;
		glm::mat4 inverseView;
		glm::mat4 projectionMatrix;

		virtual void computeKeyboardInput(const InputManager& input);
		virtual void computeMouseInput(const InputManager& input);

		glm::mat4 computeViewMatrix() const;
		virtual glm::mat4 computeProjectionMatrix() const = 0;
	};
}

#endif
