#ifndef SCENECONTROLOBJECT_H
#define SCENECONTROLOBJECT_H

#include "../inputmanager.h"
#include "../window.h"

namespace geeL {

	class Camera;
	class RenderScene;

	//Base class for scripting objects that can control behaviour and objects of a scene
	class SceneControlObject {

	public:
		SceneControlObject(RenderScene& scene) : scene(scene) {}

		virtual void init() = 0;
		virtual void draw(const Camera& camera) = 0;
		virtual void handleInput(const InputManager& input) {}
		virtual void quit() = 0;

	protected:
		RenderScene& scene;
	};
}

#endif