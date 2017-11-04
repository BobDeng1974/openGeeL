#ifndef POSTRENDERER_H
#define POSTRENDERER_H

namespace geeL {

	class DynamicRenderTexture;
	class PostProcessingEffect;
	class RenderTexture;
	class SSAO;
	class WorldMapRequester;


	//Interface for drawing/rendering classes that can 
	//draw post processing effects to screen
	class PostEffectDrawer {

	public:
		virtual void addEffect(PostProcessingEffect& effect) = 0;
		virtual void addEffect(PostProcessingEffect& effect, RenderTexture& texture) = 0;
		
		virtual void addRequester(WorldMapRequester& requester) = 0;
		virtual void addRenderTexture(DynamicRenderTexture& texture) = 0;


		template<typename... WorldMapRequesters>
		void addEffect(PostProcessingEffect& effect, WorldMapRequesters& ...requester);
		
		template<typename... WorldMapRequesters>
		void addEffect(PostProcessingEffect& effect, RenderTexture& texture, WorldMapRequesters& ...requester);
		

		template<typename... WorldMapRequesters>
		void addRequester(WorldMapRequester& requester, WorldMapRequesters& ...other);

	};


	template<typename ...WorldMapRequesters>
	inline void PostEffectDrawer::addEffect(PostProcessingEffect& effect, WorldMapRequesters& ...requester) {
		addEffect(effect);
		addRequester(requester...);
	}

	template<typename ...WorldMapRequesters>
	inline void PostEffectDrawer::addEffect(PostProcessingEffect& effect, RenderTexture& texture, WorldMapRequesters& ...requester) {
		addEffect(effect, texture);
		addRequester(requester...);
	}

	template<typename ...WorldMapRequesters>
	inline void PostEffectDrawer::addRequester(WorldMapRequester& requester, WorldMapRequesters& ...other) {
		addRequester(requester);
		addRequester(other...);
	}

}

#endif
