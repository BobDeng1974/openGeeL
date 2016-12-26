#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <list>
#include "../utility/screenquad.h"
#include "../utility/framebuffer.h"
#include "../utility/gbuffer.h"
#include "../renderer.h"

namespace geeL {

class Camera;
class Shader;
class SSAO;
class PostProcessingEffect;
class WorldPostProcessingEffect;

class DeferredRenderer : public Renderer {

public:
	DeferredRenderer(RenderWindow* window, InputManager* inputManager);
	DeferredRenderer(RenderWindow* window, InputManager* inputManager, SSAO* ssao);
	~DeferredRenderer();

	virtual void init();
	virtual void render();
	virtual void draw();
	virtual void handleInput();
	virtual void setScene(RenderScene& scene);

	//Add new post processing effect to renderer. 
	//Note: Effects will be drawn in reverse adding order
	void addEffect(PostProcessingEffect& effect);
	void addEffect(WorldPostProcessingEffect& effect);

private:
	bool geometryPass = true;
	Shader* deferredShader;
	std::list<PostProcessingEffect*> effects;
	ScreenQuad screen;
	GBuffer gBuffer;
	FrameBuffer frameBuffer1;
	FrameBuffer frameBuffer2;

	SSAO* ssao = nullptr;
	FrameBuffer* ssaoBuffer = nullptr;

	//Link all world information to given post effect
	void linkWorldInformation(WorldPostProcessingEffect& effect);
};

}

#endif