#ifndef SCENECONTROLOBJECT_H
#define SCENECONTROLOBJECT_H

#include "../inputmanager.h"
#include "../window.h"

namespace geeL {

	class SceneCamera;
	class RenderScene;

	//Base class for scripting objects that can control behaviour and objects of a scene
	class SceneControlObject {

	public:
		SceneControlObject(RenderScene& scene) : scene(scene) {}

		

	protected:
		RenderScene& scene;

		virtual void init() = 0;

	};
}

#endif