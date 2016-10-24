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
	unsigned int fbo = 0;

	virtual void draw() = 0;

	//FBO this object should be drawn to.
	//If the object used other fbo's itself it should activate
	//this one afterwards
	void setFBO(unsigned int fbo) {
		this->fbo = fbo;
	}
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
