#ifndef CAMERA_H
#define CAMERA_H

#include <functional>
#include <string>
#include <vector>
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

		//Defines a movable camera
		Camera(Transform& transform, float speed, float sensitivity, float nearClip, 
			float farClip, const std::string& name = "Camera");

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

		const float getNearPlane() const;
		const float getFarPlane() const;

		void setNearPlane(float near);
		void setFarPlane(float far);

		void addViewingPlaneChangeListener(std::function<void(float, float)> listener);
		void removeViewingPlaneChangeListener(std::function<void(float, float)> listener);

		//Returns view borders for given frustum 
		virtual std::vector<glm::vec3> getViewBorders(float near, float far) const = 0;

	protected:
		float speed, sensitivity;
		float nearClip, farClip;
		glm::mat4 viewMatrix;
		glm::mat4 inverseView;
		glm::mat4 projectionMatrix;
		std::list<std::function<void(float, float)>> callbacks;

		virtual void computeKeyboardInput(const InputManager& input);
		virtual void computeMouseInput(const InputManager& input);

		glm::mat4 computeViewMatrix() const;
		virtual glm::mat4 computeProjectionMatrix() const = 0;

		void onViewingPlaneChange();
	};
}

#endif
