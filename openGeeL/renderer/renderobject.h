#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "inputmanager.h"
#include "window.h"

namespace geeL {

class Camera;
class RenderScene;

class SceneObject {

public:
	SceneObject(RenderScene& scene) : scene(scene) {}

	virtual void init() = 0;
	virtual void draw(const Camera& camera) = 0;
	virtual void handleInput(const InputManager& input) {}
	virtual void quit() = 0;

protected:
	RenderScene& scene;
};


}


#endif