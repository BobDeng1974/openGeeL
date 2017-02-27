#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <list>
#include <vector>
#include "../utility/screenquad.h"
#include "../utility/framebuffer.h"
#include "../utility/gbuffer.h"
#include "../utility/worldinformation.h"
#include "../renderer.h"

namespace geeL {

	class Camera;
	class Shader;
	class SSAO;
	class PostProcessingEffect;
	class WorldPostProcessingEffect;

	class DeferredRenderer : public Renderer, public WorldInformationProvider {

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
		void addEffect(PostProcessingEffect& effect, WorldInformationRequester& requester);
		void addEffect(PostProcessingEffect& effect, std::list<WorldInformationRequester*> requester);

		virtual void addRequester(WorldInformationRequester& requester);
		virtual void linkInformation() const;

	private:
		int toggle;
		bool geometryPass = true;
		Shader* deferredShader;
		std::vector<PostProcessingEffect*> effects;
		std::list<WorldInformationRequester*> requester;
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

		//Initialize start of rendering process
		void renderInit();

		unsigned int linkImageBuffer() const;
		void handleInput(GLFWwindow* window, int key, int scancode, int action, int mode);

		//Toggle through all framebuffers for screen display 
		void toggleBuffer(bool next);
	};
}

#endif