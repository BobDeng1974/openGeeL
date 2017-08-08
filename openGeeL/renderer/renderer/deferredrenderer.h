#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <list>
#include <vector>
#include "framebuffer/stackbuffer.h"
#include "utility/worldinformation.h"
#include "utility/rendertime.h"
#include "postrenderer.h"
#include "renderer.h"

namespace geeL {

	class Camera;
	class DefaultPostProcess;
	class DynamicRenderTexture;
	class GBuffer;
	class MaterialFactory;
	class RenderTexture;
	class SceneRender;
	class SceneCamera;
	class SSAO;
	class PostProcessingEffect;
	class Texture;


	class DeferredRenderer : public Renderer, public WorldMapProvider, public PostEffectDrawer {

	public:
		DeferredRenderer(RenderWindow& window, Input& inputManager, SceneRender& lighting,
			RenderContext& context, DefaultPostProcess& def, GBuffer& gBuffer);

		~DeferredRenderer();

		virtual void runStart();
		virtual void run();
		virtual void draw();
		virtual void draw(const Camera& camera, const FrameBuffer& buffer);
		virtual void drawSimple(const Camera& camera);

		virtual void addSSAO(SSAO& ssao);

		//Add new post processing effect to renderer. 
		virtual void addEffect(PostProcessingEffect& effect);
		virtual void addEffect(PostProcessingEffect& effect, WorldMapRequester& requester);
		virtual void addEffect(PostProcessingEffect& effect, std::list<WorldMapRequester*> requester);

		virtual void addRenderTexture(DynamicRenderTexture& texture);

		virtual void addRequester(WorldMapRequester& requester);
		virtual void linkInformation() const;
		virtual std::map<WorldMaps, const Texture*> getMaps() const;

	private:
		int toggle;
		
		std::vector<PostProcessingEffect*> effects;
		std::list<DynamicRenderTexture*> renderTextures;
		std::list<WorldMapRequester*> requester;

		RenderTexture* texture1;
		RenderTexture* texture2;
		GBuffer& gBuffer;
		StackBuffer stackBuffer;

		SceneRender& lighting;
		SSAO* ssao;
		RenderTexture* ssaoTexture = nullptr;
		PostProcessingEffect* isolatedEffect = nullptr;

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
		void handleInput(int key, int scancode, int action, int mode);

		//Toggle through all framebuffers for screen display 
		void toggleBuffer(bool next);
	};
}

#endif