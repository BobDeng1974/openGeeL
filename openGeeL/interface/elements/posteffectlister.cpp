#include "../guiwrapper.h"
#include "../guisnippets.h"
#include "postsnippets.h"
#include "posteffectlister.h"


namespace geeL {

	PostProcessingEffectLister::~PostProcessingEffectLister() {
		for (auto it = snippets.begin(); it != snippets.end(); it++) {
			delete *it;
		}
	}


	void PostProcessingEffectLister::draw(GUIContext* context) {
		
		if (nk_begin(context, "Post processing", nk_rect(20, 400, 350, 350),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

			if (nk_tree_push(context, NK_TREE_NODE, "Effects", NK_MINIMIZED)) {

				unsigned int counter = 1;
				for (auto it = snippets.begin(); it != snippets.end(); it++) {
					GUISnippet& snippet = **it;

					std::string name = std::to_string(counter) + ". " + snippet.toString();
					if (nk_tree_push(context, NK_TREE_NODE, name.c_str(), NK_MINIMIZED)) {
						snippet.draw(context);
						nk_tree_pop(context);
					}

					counter++;
				}

				nk_tree_pop(context);
			}
		}

		nk_end(context);
	}

	void PostProcessingEffectLister::addDefaultEffect(DefaultPostProcess& def) {
		DefaultSnippet* snippet = new DefaultSnippet(def);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::addBlurredEffect(BlurredPostEffect& effect, GUISnippet& effectSnippet) {
		BlurredEffectSnippet* snippet = new BlurredEffectSnippet(effect, effectSnippet);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::addBloom(Bloom& bloom) {
		BloomSnippet* snippet = new BloomSnippet(bloom);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::addColorCorrection(ColorCorrection& color) {
		ColorCorrectionSnippet* snippet = new ColorCorrectionSnippet(color);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::addDepthOfField(DepthOfFieldBlurred& dof) {
		DepthOfFieldBlurredSnippet* snippet = new DepthOfFieldBlurredSnippet(dof);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::addGodRay(GodRay& ray) {
		GodRaySnippet* snippet = new GodRaySnippet(ray);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::addVolumetricLight(VolumetricLight& light) {
		VolumetricLightSnippet* snippet = new VolumetricLightSnippet(light);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::addSSAO(SSAO& ssao) {
		SSAOSnippet* snippet = new SSAOSnippet(ssao);
		snippets.push_back(snippet);
	}
}