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
		~PostProcessingEffectLister();

		virtual void draw(GUIContext* context);

		void addDefaultEffect(DefaultPostProcess& def);
		void addBlurredEffect(BlurredPostEffect& effect, GUISnippet& effectSnippet);
		void addBloom(Bloom& bloom);
		void addColorCorrection(ColorCorrection& color);
		void addDepthOfField(DepthOfFieldBlurred& dof);
		void addGodRay(GodRay& ray);
		void addVolumetricLight(VolumetricLight& light);
		void addSSAO(SSAO& ssao);

	private:
		std::list<GUISnippet*> snippets;

	};
}

#endif
