#ifndef POSTEFFECTLISTER_H
#define POSTEFFECTLISTER_H

#include <list>
#include <memory>
#include "guielement.h"

namespace geeL {

	class GUISnippet;
	class Bloom;
	class BlurredPostEffect;
	class ColorCorrection;
	class DefaultPostProcess;
	class DepthOfFieldBlurred;
	class FXAA;
	class GodRay;
	class SSAO;
	class SSRR;
	class VolumetricLight;
	class VoxelConeTracer;

	class PostProcessingEffectLister : public GUIElement {

	public:
		PostProcessingEffectLister(RenderWindow& window,
			float x = 0.1f, float y = 0.1f, float width = 0.2f, float height = 0.2f);

		virtual ~PostProcessingEffectLister();

		virtual void draw(GUIContext* context);

		void add(PostEffectSnippet& snippet);
		void add(std::unique_ptr<PostEffectSnippet>& snippet);

		template<typename ...Snippets>
		void add(PostEffectSnippet& snippet, Snippets& ...snippets);

		//Add main snippet to post effect list. Memory of all given snippets will be managed
		template<typename ...Snippets>
		void add(std::unique_ptr<PostEffectSnippet> snippet, Snippets ...snippets);

		void add(DefaultPostProcess& def);
		void add(BlurredPostEffect& effect, PostEffectSnippet& effectSnippet);
		void add(BlurredPostEffect& effect, PostEffectSnippet& effectSnippet, PostEffectSnippet& blurSnippet);
		void add(BlurredPostEffect& effect, std::unique_ptr<PostEffectSnippet> effectSnippet, 
			std::unique_ptr<PostEffectSnippet> blurSnippet);

		void add(Bloom& bloom);
		void add(ColorCorrection& color);
		void add(DepthOfFieldBlurred& dof);
		void add(FXAA& fxaa);
		void add(GodRay& ray);
		void add(VolumetricLight& light);
		void add(SSAO& ssao);
		void add(SSRR& ssrr);
		void add(VoxelConeTracer& tracer);

	private:
		std::list<std::pair<bool, PostEffectSnippet*>> snippets;
		std::list<PostEffectSnippet*> externalSnippets;
		
		template<typename... Snippets>
		void addExternal(std::unique_ptr<PostEffectSnippet>& snippet, Snippets& ...snippets);
		void addExternal(std::unique_ptr<PostEffectSnippet>& snippet);

	};



	template<typename ...Snippets>
	inline void PostProcessingEffectLister::add(PostEffectSnippet& snippet, Snippets& ...snippets) {
		add(snippet);
		add(snippets...);
	}

	template<typename ...Snippets>
	inline void PostProcessingEffectLister::add(std::unique_ptr<PostEffectSnippet> snippet, Snippets ...snippets) {
		add(snippet);
		addExternal(snippets...);
	}

	template<typename ...Snippets>
	inline void PostProcessingEffectLister::addExternal(std::unique_ptr<PostEffectSnippet>& snippet, Snippets& ...snippets) {
		addExternal(snippet);
		addExternal(snippets...);
	}

}

#endif
