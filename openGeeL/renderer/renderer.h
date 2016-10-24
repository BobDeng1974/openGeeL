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


//Basic interface for rendering classes
class Drawer {

public:
	virtual void draw() = 0;
};

class Renderer : public Drawer {

public:
	Renderer(RenderWindow* window, InputManager* inputManager);

	virtual void init() = 0;

	//Render scene until termination
	virtual void render() = 0;

	//Render single frame
	virtual void draw() = 0;

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
