#ifndef WORLDINFORMATION_H
#define WORLDINFORMATION_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <map>
#include "../cameras/camera.h"
#include "../cubemapping/skybox.h"
#include "../texturing/texture.h"

namespace geeL {


	enum class WorldMaps {
		None = 0,
		DiffuseRoughness = 1,
		PositionDepth = 2,
		NormalMetallic = 4,
	};


	//Object that can request world maps
	class WorldMapRequester {

	public:
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps) = 0;

	};

	//Object that can provide world information to linked requesters
	class WorldMapProvider {

	public:
		virtual void addRequester(WorldMapRequester& requester) = 0;
		virtual void linkInformation() const = 0;

	};

	//Object that can request current scene information
	class SceneRequester {

	public:
		virtual void updateInformation(SceneCamera& camera, Skybox& skybox) = 0;

	};

	//Special scene requester that stores current scene camera
	class CameraRequester : public SceneRequester {

	public:
		virtual void updateInformation(SceneCamera& camera, Skybox& skybox) {
			this->camera = &camera;
		}

	protected:
		SceneCamera* camera = nullptr;
	};



	inline WorldMaps operator|(WorldMaps a, WorldMaps b) {
		return static_cast<WorldMaps>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline WorldMaps operator&(WorldMaps a, WorldMaps b) {
		return static_cast<WorldMaps>(static_cast<int>(a) & static_cast<int>(b));
	}

}

#endif

