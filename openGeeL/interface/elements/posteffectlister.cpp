#include "../guiwrapper.h"
#include "../snippets/guisnippets.h"
#include "../snippets/postsnippets.h"
#include "posteffectlister.h"


namespace geeL {

	PostProcessingEffectLister::PostProcessingEffectLister(RenderWindow& window, 
		float x, float y, float width, float height)
			: GUIElement(window, x, y, width, height) {}

	PostProcessingEffectLister::~PostProcessingEffectLister() {
		for (auto it = snippets.begin(); it != snippets.end(); it++) {
			delete *it;
		}
	}


	void PostProcessingEffectLister::draw(GUIContext* context) {
		
		if (nk_begin(context, "Post Processing", nk_rect(x, y, width, height),
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

	void PostProcessingEffectLister::add(DefaultPostProcess& def) {
		DefaultSnippet* snippet = new DefaultSnippet(def);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::add(BlurredPostEffect& effect, GUISnippet& effectSnippet) {
		BlurredEffectSnippet* snippet = new BlurredEffectSnippet(effect, effectSnippet);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::add(Bloom& bloom) {
		BloomSnippet* snippet = new BloomSnippet(bloom);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::add(ColorCorrection& color) {
		ColorCorrectionSnippet* snippet = new ColorCorrectionSnippet(color);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::add(DepthOfFieldBlurred& dof) {
		DepthOfFieldBlurredSnippet* snippet = new DepthOfFieldBlurredSnippet(dof);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::add(GodRay& ray) {
		GodRaySnippet* snippet = new GodRaySnippet(ray);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::add(VolumetricLight& light) {
		VolumetricLightSnippet* snippet = new VolumetricLightSnippet(light);
		snippets.push_back(snippet);
	}

	void PostProcessingEffectLister::add(SSAO& ssao) {
		SSAOSnippet* snippet = new SSAOSnippet(ssao);
		snippets.push_back(snippet);
	}
}