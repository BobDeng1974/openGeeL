#include "guiwrapper.h"
#include "shadowmapping/shadowmapadapter.h"
#include "shadowadapterdebug.h"

namespace geeL {

	ShadowAdapterDebug::ShadowAdapterDebug(ShadowmapAdapter & adapter)
		: adapter(adapter) {}


	void ShadowAdapterDebug::draw(GUIContext* context) {
		if (nk_tree_push(context, NK_TREE_NODE, "Shadowmap Adapter", NK_MAXIMIZED)) {

			float d = GUISnippets::drawBarFloatLogarithmic(context, adapter.getDepthScale(), 0.01f, 128.f, 0.1f, "Depth");
			adapter.setDepthScale(d);

			float a = GUISnippets::drawBarFloat(context, adapter.getAttenuationScale(), 0.01f, 1.f, 0.001f, "Attenuation");
			adapter.setAttenuationScale(a);

			float b = GUISnippets::drawBarFloat(context, adapter.getBaseSizeScale(), 0.1f, 5.f, 0.01f, "Base Size");
			adapter.setBaseSizeScale(b);

			nk_tree_pop(context);
		}

	}

	std::string ShadowAdapterDebug::toString() const {
		return "Shadowmap adapter debug window";
	}

}