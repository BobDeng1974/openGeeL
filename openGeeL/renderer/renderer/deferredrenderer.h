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
	class TextureProvider;

	using PostEffectRender = std::pair<RenderTexture*, PostProcessingEffect*>;


	class DeferredRenderer : public Renderer, public PostEffectDrawer {

	public:
		DeferredRenderer(RenderWindow& window, 
			TextureProvider& provider, 
			SceneRender& lighting,
			RenderContext& context, 
			DefaultPostProcess& def, 
			GBuffer& gBuffer);

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

		virtual void addRenderTexture(DynamicRenderTexture& texture);
		void addFBuffer(ForwardBuffer& buffer);

		//Set image that gets drawn to screen. Picks default 
		//image if given texture is NULL
		void setScreenImage(const Texture* const texture = nullptr);

		//Returns all available drawing buffers.
		//This included buffers for final image, albedo, normals,
		//positions, occlusion and emission
		std::vector<const Texture*> getBuffers();

		StackBuffer& getStackbuffer();

	private:
		std::vector<PostEffectRender> earlyEffects;
		std::vector<PostEffectRender> intermediateEffects;
		std::vector<PostEffectRender> lateEffects;
		std::vector<PostEffectRender> externalEffects;

		std::list<DynamicRenderTexture*> renderTextures;

		TextureProvider& provider;
		GBuffer& gBuffer;
		ForwardBuffer* fBuffer;
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
		void initEffects();
		
		void iterEffects(std::vector<PostEffectRender>& effects, std::function<void(PostProcessingEffect&)> function);
		void drawEffects(std::vector<PostEffectRender>& effects);
		
		//Update shader bindings of all contained post effects
		void updateEffectBindings();

		void lightingPass();
		bool hasForwardPass() const;

	};

}

#endif