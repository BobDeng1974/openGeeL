#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <list>
#include <vector>
#include "framebuffer/stackbuffer.h"
#include "postprocessing/postprocessing.h"
#include "utility/worldinformation.h"
#include "utility/rendertime.h"
#include "postrenderer.h"
#include "renderer.h"

namespace geeL {

	class Camera;
	class DefaultPostProcess;
	class DynamicRenderTexture;
	class ForwardBuffer;
	class GBuffer;
	class MaterialFactory;
	class RenderTexture;
	class SceneRender;
	class SceneCamera;
	class SSAO;
	class PostProcessingEffect;
	class Texture;
	class TransparentOIDBuffer;

	using PostEffectRender = std::pair<RenderTexture*, PostProcessingEffect*>;


	class DeferredRenderer : public Renderer, public WorldMapProvider, public PostEffectDrawer {

	public:
		DeferredRenderer(RenderWindow& window, Input& inputManager, SceneRender& lighting,
			RenderContext& context, DefaultPostProcess& def, GBuffer& gBuffer);
		virtual ~DeferredRenderer();

		virtual void runStart();
		virtual void run();
		virtual void draw();
		virtual void draw(const Camera& camera, const FrameBuffer& buffer);

		//Render current frame of scene with classic forward rendering
		virtual void drawForward(const Camera& camera);

		//Add new post processing effect to renderer. 
		virtual void addEffect(PostProcessingEffect& effect, DrawTime time);
		virtual void addEffect(PostProcessingEffect& effect, RenderTexture& texture);
		void addEffect(SSAO& ssao);


		virtual void addRequester(WorldMapRequester& requester);
		virtual void addRenderTexture(DynamicRenderTexture& texture);

		virtual void linkInformation() const;
		virtual std::map<WorldMaps, const Texture*> getMaps() const;

		void addFBuffer(ForwardBuffer& buffer);
		void addTBuffer(TransparentOIDBuffer& buffer);

		StackBuffer& getStackbuffer();

	private:
		int toggle;
		
		std::vector<PostEffectRender> earlyEffects;
		std::vector<PostEffectRender> intermediateEffects;
		std::vector<PostEffectRender> lateEffects;
		std::vector<PostEffectRender> externalEffects;

		std::list<DynamicRenderTexture*> renderTextures;
		std::list<WorldMapRequester*> requester;

		RenderTexture* texture1;
		RenderTexture* texture2;
		GBuffer& gBuffer;
		ForwardBuffer* fBuffer;
		TransparentOIDBuffer* tBuffer;
		StackBuffer stackBuffer;

		SceneRender& lighting;
		SSAO* ssao;
		PostProcessingEffectFS fallbackEffect;
		DefaultPostProcess& defaultEffect;

		std::function<void()> geometryPassFunction;
		std::function<void()> lightingPassFunction;

		DeferredRenderer(const DeferredRenderer& other) = delete;
		DeferredRenderer& operator= (const DeferredRenderer& other) = delete;

		void init();
		void indexEffects();
		void indexEffect(PostEffectRender& current, PostEffectRender* previous, RenderTexture* firstTexture);

		RenderTexture* indexEffectList(std::vector<PostEffectRender>& effects, RenderTexture* firstTexture);
		RenderTexture* drawEffects(std::vector<PostEffectRender>& effects, RenderTexture* lastTexture);

		void lightingPass();
		bool hasForwardPass() const;

		void handleInput(int key, int scancode, int action, int mode);

		//Toggle through all framebuffers for screen display 
		void toggleBuffer(bool next);

	};

}

#endif