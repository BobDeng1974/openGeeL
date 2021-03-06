#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include <list>
#include "camera.h"

namespace geeL {

	class Transform;

	class PerspectiveCamera : public SceneCamera {

	public:		
		PerspectiveCamera(Transform& transform, 
			float fov, 
			unsigned int width, 
			unsigned int height, 
			float nearClip, 
			float farClip, 
			const std::string& name = "PerspectiveCamera");

		float getFieldOfView() const;
		void setFieldOfView(float fov);

		virtual std::vector<glm::vec3> getViewBorders(float near, float far) const;

		virtual const ViewFrustum& getFrustum() const;

	protected:
		virtual ViewFrustum& getFrustum();

	private:
		PerspectiveFrustum frustum;

		virtual void computeProjectionMatrix();

	};
}

#endif
