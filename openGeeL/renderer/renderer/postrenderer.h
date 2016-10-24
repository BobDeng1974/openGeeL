#ifndef POSTRENDERER_H
#define POSTRENDERER_H

#include "../postprocessing/postprocessscreen.h"
#include "../utility/framebuffer.h"
#include "../renderer.h"

typedef unsigned int GLuint;

namespace geeL {

class Camera;
class Shader;
class PostProcessingEffect;

class PostProcessingRenderer : public Renderer {

public:
	PostProcessingRenderer(RenderWindow* window, InputManager* inputManager);

	virtual void init();
	virtual void render();
	virtual void draw();
	virtual void handleInput();

	void setEffect(PostProcessingEffect& effect);

private:
	Shader* postShader;
	PostProcessingEffect* effect;
	PostProcessingScreen screen;
	FrameBuffer frameBuffer;
};

}


#endif
