#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include "../utility/screenquad.h"
#include "../utility/framebuffer.h"
#include "../utility/gbuffer.h"
#include "../renderer.h"

namespace geeL {

class Camera;
class Shader;
class PostProcessingEffect;

class DeferredRenderer : public Renderer {

public:
	DeferredRenderer(RenderWindow* window, InputManager* inputManager);
	~DeferredRenderer();

	virtual void init();
	virtual void render();
	virtual void draw();
	virtual void handleInput();

	void setEffect(PostProcessingEffect& effect);

private:
	bool geometryPass = true;
	Shader* deferredShader;
	PostProcessingEffect* effect;
	ScreenQuad screen;
	GBuffer gBuffer;
	FrameBuffer frameBuffer;
};

}

#endif