#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <vector>
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
		DeferredRenderer(RenderWindow* window, InputManager* inputManager, SSAO* ssao, float ssaoResolution = 1.f);
		~DeferredRenderer();

		virtual void init();
		virtual void render();
		virtual void draw();
		virtual void handleInput();
		virtual void setScene(RenderScene& scene);

		//Add new post processing effect to renderer. 
		void addEffect(PostProcessingEffect& effect);
		void addEffect(WorldPostProcessingEffect& effect);

	private:
		int toggle;
		bool geometryPass = true;
		Shader* deferredShader;
		std::vector<PostProcessingEffect*> effects;
		ScreenQuad screen;
		GBuffer gBuffer;
		FrameBuffer frameBuffer1;
		FrameBuffer frameBuffer2;

		unsigned int defaultBuffer;
		float ssaoResolution;
		SSAO* ssao = nullptr;
		ScreenQuad* ssaoScreen = nullptr;
		FrameBuffer* ssaoBuffer = nullptr;
		PostProcessingEffect* isolatedEffect = nullptr;

		//Link all world information to given post effect
		void linkWorldInformation(WorldPostProcessingEffect& effect);

		void handleInput(GLFWwindow* window, int key, int scancode, int action, int mode);

		//Toggle through all framebuffers for screen display 
		void toggleBuffer(bool next);
	};
}

#endif