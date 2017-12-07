#include "guiwrapper.h"
#include "snippets/guisnippets.h"
#include "snippets/postsnippets.h"
#include "posteffectlister.h"


namespace geeL {

	PostProcessingEffectLister::PostProcessingEffectLister(RenderWindow& window, 
		float x, 
		float y, 
		float width, 
		float height)
			: GUIElement(window, x, y, width, height) {}

	PostProcessingEffectLister::~PostProcessingEffectLister() {
		for (auto it(snippets.begin()); it != snippets.end(); it++) {
			auto pair = *it;

			if(pair.first)
				delete pair.second;
		}

		for (auto it(externalSnippets.begin()); it != externalSnippets.end(); it++)
			delete *it;
			
	}


	void PostProcessingEffectLister::draw(GUIContext* context) {
		
		if (nk_begin(context, "Post Processing", nk_rect(x, y, width, height),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

			if (nk_tree_push(context, NK_TREE_NODE, "Effects", NK_MINIMIZED)) {

				unsigned int counter = 1;
				for (auto it(snippets.begin()); it != snippets.end(); it++) {
					GUISnippet& snippet = *it->second;

					unsigned int snippetID = reinterpret_cast<unsigned int>(&snippet);

					std::string name = std::to_string(counter) + ". " + snippet.toString();
					if (nk_tree_push_id(context, NK_TREE_NODE, name.c_str(), NK_MINIMIZED, snippetID)) {
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

	void PostProcessingEffectLister::add(PostEffectSnippet& snippet) {
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(false, &snippet));
	}

	void PostProcessingEffectLister::add(std::unique_ptr<PostEffectSnippet>& snippet) {
		PostEffectSnippet* s = snippet.release();
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, s));

	}

	void PostProcessingEffectLister::add(DefaultPostProcess& def) {
		DefaultSnippet* snippet = new DefaultSnippet(def);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(BlurredPostEffect& effect, PostEffectSnippet& effectSnippet) {
		BlurredEffectSnippet* snippet = new BlurredEffectSnippet(effect, effectSnippet);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(BlurredPostEffect& effect, PostEffectSnippet& effectSnippet, PostEffectSnippet& blurSnippet) {
		BlurredEffectSnippet* snippet = new BlurredEffectSnippet(effect, effectSnippet);
		snippet->setBlurSnippet(blurSnippet);

		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(BlurredPostEffect& effect, std::unique_ptr<PostEffectSnippet> effectSnippet, 
		std::unique_ptr<PostEffectSnippet> blurSnippet) {

		PostEffectSnippet* es = effectSnippet.release();
		PostEffectSnippet* bs = blurSnippet.release();

		externalSnippets.push_back(es);
		externalSnippets.push_back(bs);

		BlurredEffectSnippet* snippet = new BlurredEffectSnippet(effect, *es);
		snippet->setBlurSnippet(*bs);

		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(Bloom& bloom) {
		BloomSnippet* snippet = new BloomSnippet(bloom);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(ColorCorrection& color) {
		ColorCorrectionSnippet* snippet = new ColorCorrectionSnippet(color);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(DepthOfFieldBlurred& dof) {
		DepthOfFieldBlurredSnippet* snippet = new DepthOfFieldBlurredSnippet(dof);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(FXAA& fxaa) {
		FXAASnippet* snippet = new FXAASnippet(fxaa);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(GodRay& ray) {
		GodRaySnippet* snippet = new GodRaySnippet(ray);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(VolumetricLight& light) {
		VolumetricLightSnippet* snippet = new VolumetricLightSnippet(light);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(SSAO& ssao) {
		SSAOSnippet* snippet = new SSAOSnippet(ssao);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(SSRR& ssrr) {
		SSRRSnippet* snippet = new SSRRSnippet(ssrr);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::add(VoxelConeTracer& tracer) {
		ConeTracerSnippet* snippet = new ConeTracerSnippet(tracer);
		snippets.push_back(std::pair<bool, PostEffectSnippet*>(true, snippet));
	}

	void PostProcessingEffectLister::addExternal(std::unique_ptr<PostEffectSnippet>& snippet) {
		externalSnippets.push_back(snippet.release());
	}

}