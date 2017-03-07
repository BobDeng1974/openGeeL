#ifndef POSTEFFECTLISTER_H
#define POSTEFFECTLISTER_H

#include <list>

#include "../guielement.h"

namespace geeL {

	class GUISnippet;
	class Bloom;
	class BlurredPostEffect;
	class ColorCorrection;
	class DefaultPostProcess;
	class DepthOfFieldBlurred;
	class GodRay;
	class SSAO;
	class VolumetricLight;

	class PostProcessingEffectLister : public GUIElement {

	public:
		PostProcessingEffectLister(RenderWindow& window,
			float x = 0.1f, float y = 0.1f, float width = 0.2f, float height = 0.2f);

		~PostProcessingEffectLister();

		virtual void draw(GUIContext* context);

		void add(DefaultPostProcess& def);
		void add(BlurredPostEffect& effect, GUISnippet& effectSnippet);
		void add(Bloom& bloom);
		void add(ColorCorrection& color);
		void add(DepthOfFieldBlurred& dof);
		void add(GodRay& ray);
		void add(VolumetricLight& light);
		void add(SSAO& ssao);

	private:
		std::list<GUISnippet*> snippets;

	};
}

#endif
