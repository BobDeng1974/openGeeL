#ifndef POSTRENDERER_H
#define POSTRENDERER_H

namespace geeL {

	class DynamicRenderTexture;
	class PostProcessingEffect;
	class RenderTexture;
	class SSAO;
	class WorldMapRequester;


	//Specifies a time to be drawn during rendering process
	//External: Outside regular rendering pipeline
	//Early: Directly after geometry pass
	//Intermediate: After transparency pass (Default)
	//Late: After generic pass
	enum class DrawTime {
		External,
		Early,
		Intermediate,
		Late

	};


	//Interface for drawing/rendering classes that can 
	//draw post processing effects to screen
	class PostEffectDrawer {

	public:
		virtual void addEffect(PostProcessingEffect& effect, DrawTime time = DrawTime::Intermediate) = 0;
		virtual void addRenderTexture(DynamicRenderTexture& texture) = 0;

		template<typename... PostProcessingEffects>
		void addEffects(DrawTime time, PostProcessingEffect& effect, PostProcessingEffects& ...effects);

	};



	template<typename ...PostProcessingEffects>
	inline void PostEffectDrawer::addEffects(DrawTime time, 
		PostProcessingEffect& effect, PostProcessingEffects& ...effects) {

		addEffect(effect, time);
		addEffects(time, effects...);
	}

}

#endif
