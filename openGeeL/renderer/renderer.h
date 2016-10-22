#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

namespace geeL {

class RenderWindow;
class RenderScene;
class SceneControlObject;
class InputManager;
class LightManager;
class MeshDrawer;
class ShaderManager;


class Renderer {

public:
	Renderer(RenderWindow* window, InputManager* inputManager);

	virtual void init() = 0;
	virtual void render() = 0;
	virtual void renderFrame() = 0;
	virtual void handleInput() = 0;

	virtual void addObject(SceneControlObject* obj);
	virtual void initObjects();
	
	void setScene(const RenderScene& scene);
	void setShaderManager(const ShaderManager& manager);


protected:
	RenderWindow* window;
	InputManager* inputManager;

	const RenderScene* scene;
	const ShaderManager* shaderManager;
	
	std::vector<SceneControlObject*> objects;

};

}


#endif
