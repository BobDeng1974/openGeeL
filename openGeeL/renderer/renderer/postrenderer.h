#ifndef POSTRENDERER_H
#define POSTRENDERER_H

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
	virtual void renderFrame();
	virtual void handleInput();

	void setEffect(PostProcessingEffect& effect);

	void setCamera(Camera* camera);

private:
	GLuint frameBuffer, colorBuffer, screenVAO, screenVBO;
	Camera* currentCamera;
	Shader* postShader;
	PostProcessingEffect* effect;

	void initFrameBuffer();
	void initScreenQuad();

};

}


#endif
