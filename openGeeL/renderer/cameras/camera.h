#ifndef CAMERA_H
#define CAMERA_H

#include <functional>
#include <string>
#include <vector>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "structures/viewfrustum.h"
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
		virtual const ViewFrustum& getFrustum() const = 0;

		virtual void setViewMatrix(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);
		virtual void setViewMatrix(const glm::mat4& view);
		virtual void setProjectionMatrix(const glm::mat4& projection);

	protected:
		mutable std::mutex cameraMutex;

		Camera(Transform& transform, const std::string& name = "Camera");


		virtual ViewFrustum& getFrustum() = 0;

	private:
		glm::vec3 originViewSpace;
		glm::mat4 viewMatrix;
		glm::mat4 inverseView;
		glm::mat4 projectionMatrix;

	};


	//Simple camera whose view and projection matrices must be computed and set manually
	class ManualCamera : public Camera {

	public:
		ManualCamera(Transform& transform, std::unique_ptr<ViewFrustum> frustum, 
			const std::string&name = "Camera");

		//Update view matrix and frustum with transformational changes
		void injectTransform();

		virtual void setViewMatrix(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);
		virtual void setViewMatrix(const glm::mat4& view);
		virtual void setProjectionMatrix(const glm::mat4& projection);

		virtual const ViewFrustum& getFrustum() const;
		void setFrustum(std::unique_ptr<ViewFrustum> frustum);

	protected:
		virtual ViewFrustum& getFrustum();

	private:
		std::unique_ptr<ViewFrustum> frustum;

		void updateFrustum();

	};


	//Movable camera that offers additional scene information
	class SceneCamera : public Camera {

	public:
		//Defines a movable camera
		SceneCamera(Transform& transform, const std::string& name = "Camera");

		//Update view and projection matrices
		virtual void lateUpdate();

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
		std::list<std::function<void(const SceneCamera&)>> callbacks;

		void computeViewMatrix();
		virtual void computeProjectionMatrix() = 0;

		void onViewingPlaneChange();

	};


}

#endif
