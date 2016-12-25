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
	unsigned int parentFBO = 0;

	virtual void draw() = 0;

	//FBO this object should be drawn to.
	//If the object used other fbo's itself it should activate
	//this one afterwards
	void setParentFBO(unsigned int fbo) {
		this->parentFBO = fbo;
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
	
	virtual void setScene(RenderScene& scene);
	void setShaderManager(const ShaderManager& manager);


protected:
	RenderWindow* window;
	InputManager* inputManager;

	RenderScene* scene;
	const ShaderManager* shaderManager;
	
	std::vector<SceneControlObject*> objects;

};

}


#endif
