#ifndef CAMERA_H
#define CAMERA_H

#include <functional>
#include <string>
#include <vector>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "utility/screeninfo.h"
#include "objectwrapper.h"
#include "sceneobject.h"


namespace geeL {

	class Input;
	class Transform;
	class RenderShader;
	class SceneShader;
	class Shader;

	typedef int ShaderLocation;



	//Base class for all camera objects that contains only transformational information
	class Camera : public SceneObject {

	public:
		float depth;
		ScreenInfo info;
		glm::vec3 center;

		glm::mat4 getViewMatrix() const;
		glm::mat4 getInverseViewMatrix() const;
		glm::mat4 getProjectionMatrix() const;

		void bindViewMatrix(const Shader& shader, ShaderLocation location) const;
		void bindViewMatrix(const Shader& shader, const std::string& name) const;
		void bindInverseViewMatrix(const Shader& shader, ShaderLocation location) const;
		void bindInverseViewMatrix(const Shader& shader, const std::string& name) const;
		void bindProjectionMatrix(const Shader& shader, ShaderLocation location) const;
		void bindProjectionMatrix(const Shader& shader, const std::string& name) const;

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

		bool inView(const glm::vec3& position) const;
		bool isBehind(const glm::vec3& position) const;

		void setViewMatrix(const glm::mat4& view);
		void setProjectionMatrix(const glm::mat4& projection);

	protected:
		mutable std::mutex cameraMutex;

		Camera(Transform& transform, const std::string& name = "Camera");

	private:
		glm::vec3 originViewSpace;
		glm::mat4 viewMatrix;
		glm::mat4 inverseView;
		glm::mat4 projectionMatrix;

	};


	//Simple camera whose view and projection matrices must be computed and set manually
	class SimpleCamera : public Camera {

	public:
		SimpleCamera(Transform& transform, const std::string&name = "Camera");

	};


	//Movable camera that offers additional scene information
	class SceneCamera : public Camera {

	public:
		//Defines a movable camera
		SceneCamera(Transform& transform, float nearClip, float farClip, const std::string& name = "Camera");

		//Update view and projection matrices
		virtual void lateUpdate();

		//Update position and depth of center pixel of camera view
		void updateDepth(const ScreenInfo& info);

		const float getNearPlane() const;
		const float getFarPlane() const;

		void setNearPlane(float near);
		void setFarPlane(float far);

		//Add change listener that will be called when properties (no transformation)
		//of this camera change
		void addChangeListener(std::function<void(const SceneCamera&)> listener);

		//Returns view borders for given frustum 
		virtual std::vector<glm::vec3> getViewBorders(float near, float far) const = 0;

	protected:
		AtomicWrapper<float> nearClip, farClip;
		std::list<std::function<void(const SceneCamera&)>> callbacks;

		void computeViewMatrix();
		virtual void computeProjectionMatrix() = 0;

		void onViewingPlaneChange();

	};


}

#endif
