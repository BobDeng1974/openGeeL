#ifndef CAMERA_H
#define CAMERA_H

#include <functional>
#include <string>
#include <vector>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "utility/screeninfo.h"
#include "sceneobject.h"


namespace geeL {

	class Input;
	class Transform;
	class RenderShader;
	class SceneShader;


	//Base class for all camera objects that contains only transformational information
	class Camera : public SceneObject {

	public:
		float depth;
		ScreenInfo info;
		glm::vec3 center;

		const glm::mat4& getViewMatrix() const;
		const glm::mat4& getInverseViewMatrix() const;
		const glm::mat4& getProjectionMatrix() const;

		void bind(const SceneShader& shader) const;
		void bindPosition(const RenderShader& shader, std::string name = "cameraPosition") const;
		void uniformBind(int uniformID) const;

		//Translate vector from world to screen space of this camera
		glm::vec3 TranslateToScreenSpace(const glm::vec3& vector) const;

		//Translate vector from world to view space of this camera
		glm::vec3 TranslateToViewSpace(const glm::vec3& vector) const;

		//Transflate vector from view space of this camera to world space 
		glm::vec3 TranslateToWorldSpace(const glm::vec3& vector) const;

		const glm::vec3& GetOriginInViewSpace() const;

	protected:
		Camera(Transform& transform, const std::string& name = "Camera");

		glm::vec3 originViewSpace;
		glm::mat4 viewMatrix;
		glm::mat4 inverseView;
		glm::mat4 projectionMatrix;

	};


	//Simple camera whose view and projection matrices must be computed and set manually
	class SimpleCamera : public Camera {

	public:
		SimpleCamera(Transform& transform, const std::string&name = "Camera");

		void setViewMatrix(const glm::mat4& view);
		void setProjectionMatrix(const glm::mat4& projection);

	};


	//Movable camera that offers additional scene information
	class SceneCamera : public Camera {

	public:
		//Defines a movable camera
		SceneCamera(Transform& transform, float speed, float sensitivity, float nearClip, 
			float farClip, const std::string& name = "Camera");

		//Update view and projection matrices
		virtual void lateUpdate();

		virtual void handleInput(const Input& input);

		//Update position and depth of center pixel of camera view
		void updateDepth(const ScreenInfo& info);

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
		std::list<std::function<void(float, float)>> callbacks;

		virtual void computeKeyboardInput(const Input& input);
		virtual void computeMouseInput(const Input& input);

		glm::mat4 computeViewMatrix() const;
		virtual glm::mat4 computeProjectionMatrix() const = 0;

		void onViewingPlaneChange();
	};
}

#endif
