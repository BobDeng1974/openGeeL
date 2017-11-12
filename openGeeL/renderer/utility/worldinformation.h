#ifndef WORLDINFORMATION_H
#define WORLDINFORMATION_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <map>
#include "cameras/camera.h"
#include "cubemapping/skybox.h"
#include "texturing/texture.h"

namespace geeL {


	//Object that can request current scene information
	class SceneRequester {

	public:
		virtual void updateSkybox(Skybox& skybox) {}
		virtual void updateCamera(SceneCamera& camera) {}

	protected:
		SceneRequester() {}

	};

	//Special scene requester that stores current scene camera
	class CameraRequester : public SceneRequester {

	public:
		virtual void updateCamera(SceneCamera& camera);

		void setCamera(const Camera& camera);

	protected:
		const Camera* camera = nullptr;
	};


	inline void CameraRequester::updateCamera(SceneCamera& camera) {
		this->camera = &camera;
	}

	inline void CameraRequester::setCamera(const Camera& camera) {
		this->camera = &camera;
	}

}

#endif

