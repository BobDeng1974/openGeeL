#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <list>
#include <vector>
#include "framebuffer/stackbuffer.h"
#include "postprocessing/postprocessing.h"
#include "postprocessing/additiveeffect.h"
#include "appglobals.h"
#include "postrenderer.h"
#include "renderer.h"

namespace geeL {

	class Camera;
	class DefaultPostProcess;
	class DynamicRenderTexture;
	class ForwardBuffer;
	class GBuffer;
	class ITexture;
	class Model;
	class RenderTexture;
	class SceneRender;
	class SSAO;
	class PostProcessingEffect;
	class TextureProvider;


	class DeferredRenderer : public Renderer, public PostEffectDrawer {

	public:
		DeferredRenderer(RenderWindow& window, 
			TextureProvider& provider, 
			SceneRender& lighting,
			RenderContext& context, 
			DefaultPostProcess& def, 
			GBuffer& gBuffer,
			MeshFactory& factory);

		virtual void runStart();
		virtual void run();
		void draw(const Camera& camera, const FrameBuffer& buffer);

		//Add new post processing effect to renderer. 
		virtual void addEffect(PostProcessingEffect& effect, DrawTime time);
		virtual void addEffect(PostProcessingEffect& effect, RenderTexture& texture);

		virtual void addRenderTexture(DynamicRenderTexture& texture);
		void addFBuffer(ForwardBuffer& buffer);

		//Set image that gets drawn to screen. Picks default 
		//image if given texture is NULL
		void setScreenImage(const ITexture* const texture = nullptr);

		const TextureProvider& getTextureProvider() const;
		
	private:
		mutable std::mutex renderMutex;

		std::vector<PostProcessingEffect*> earlyEffects;
		std::vector<PostProcessingEffect*> intermediateEffects;
		std::vector<PostProcessingEffect*> lateEffects;
		std::vector<PostProcessingEffect*> externalEffects;

		std::list<DynamicRenderTexture*> renderTextures;

		TextureProvider& provider;
		GBuffer& gBuffer;
		ForwardBuffer* fBuffer;
		StackBuffer stackBuffer;
#if DIFFUSE_SPECULAR_SEPARATION
		StackBuffer separatedBuffer;
#endif

		SceneRender& lighting;
		SSAO* ssao;
		PostProcessingEffectFS fallbackEffect;
		DefaultPostProcess& defaultEffect;
		AdditiveEffect combineEffect;

		std::function<void()> geometryPassFunction;
		std::function<void()> lightingPassFunction;

		DeferredRenderer(const DeferredRenderer& other) = delete;
		DeferredRenderer& operator= (const DeferredRenderer& other) = delete;

		//Render current frame of scene with classic forward rendering
		void drawForward(const Camera& camera);
		void draw();

		void init();
		void initEffects();

		void addEffect(SSAO& ssao);
		
		void drawEffects(std::vector<PostProcessingEffect*>& effects);
		void iterEffects(std::vector<PostProcessingEffect*>& effects, 
			std::function<void(PostProcessingEffect&)> function);
		
		//Update shader bindings of all contained post effects
		void updateEffectBindings();

		void lightingPass();
		bool hasForwardPass() const;

	};

}

#endif