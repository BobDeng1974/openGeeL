#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

namespace geeL {

class RenderWindow;
class RenderObject;
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

	virtual void addObject(RenderObject* obj);
	virtual void initObjects();
	
	void setLightManager(const LightManager& manager);
	void setMeshDrawer(const MeshDrawer& drawer);
	void setShaderManager(const ShaderManager& manager);


protected:
	RenderWindow* window;
	InputManager* inputManager;

	const LightManager* lightManager;
	const MeshDrawer* meshDrawer;
	const ShaderManager* shaderManager;
	
	std::vector<RenderObject*> objects;

};

}


#endif
