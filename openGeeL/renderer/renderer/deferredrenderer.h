#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <list>
#include <vector>
#include "../primitives/screenquad.h"
#include "../framebuffer/pingpong.h"
#include "../utility/worldinformation.h"
#include "../utility/rendertime.h"
#include "../renderer.h"

namespace geeL {

	class Camera;
	class DefaultPostProcess;
	class GBuffer;
	class MaterialFactory;
	class RenderTexture;
	class SceneRender;
	class SceneCamera;
	class SSAO;
	class PostProcessingEffect;
	class Texture;
	class WorldPostProcessingEffect;

	class DeferredRenderer : public Renderer, public WorldMapProvider {

	public:
		DeferredRenderer(RenderWindow& window, InputManager& inputManager, SceneRender& lighting,
			RenderContext& context, DefaultPostProcess& def, GBuffer& gBuffer);

		~DeferredRenderer();

		virtual void render();
		virtual void draw();
		virtual void draw(const Camera& camera, const FrameBuffer& buffer);
		virtual void initSceneObjects();

		virtual void handleInput();

		void addSSAO(SSAO& ssao);

		//Add new post processing effect to renderer. 
		void addEffect(PostProcessingEffect& effect);
		void addEffect(PostProcessingEffect& effect, WorldMapRequester& requester);
		void addEffect(PostProcessingEffect& effect, std::list<WorldMapRequester*> requester);

		virtual void addRequester(WorldMapRequester& requester);
		virtual void linkInformation() const;

		const RenderTime& getRenderTime() const;

	private:
		int toggle;
		
		std::vector<PostProcessingEffect*> effects;
		std::list<WorldMapRequester*> requester;

		RenderTexture* texture1;
		RenderTexture* texture2;
		GBuffer& gBuffer;
		PingPongBuffer stackBuffer;

		SceneRender& lighting;
		SSAO* ssao;
		ColorBuffer* ssaoBuffer = nullptr;
		RenderTexture* ssaoTexture = nullptr;
		PostProcessingEffect* isolatedEffect = nullptr;

		RenderTime renderTime;

		std::function<void()> geometryPassFunc;
		std::function<void()> lightingPassFunc;

		DeferredRenderer(const DeferredRenderer& other) = delete;
		DeferredRenderer& operator= (const DeferredRenderer& other) = delete;

		void init();
		void initDefaultEffect();

		void geometryPass();
		void lightingPass();
		void lightingPass(const Camera& camera);
		void forwardPass();

		void linkImageBuffer(PostProcessingEffect& effect);
		void handleInput(GLFWwindow* window, int key, int scancode, int action, int mode);

		//Toggle through all framebuffers for screen display 
		void toggleBuffer(bool next);
	};
}

#endif