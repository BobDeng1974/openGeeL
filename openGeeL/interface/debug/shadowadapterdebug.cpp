#include "guiwrapper.h"
#include "shadowmapping/shadowmapadapter.h"
#include "shadowadapterdebug.h"

namespace geeL {

	ShadowAdapterDebug::ShadowAdapterDebug(ShadowmapAdapter & adapter)
		: adapter(adapter) {}


	void ShadowAdapterDebug::draw(GUIContext* context) {
		if (nk_tree_push(context, NK_TREE_NODE, "Shadowmap Adapter", NK_MAXIMIZED)) {

			float d = GUISnippets::drawBarFloatLogarithmic(context, adapter.getDepthScale(), 0.0f, 128.f, 0.1f, "Depth Scale");
			adapter.setDepthScale(d);

			float a = GUISnippets::drawBarFloat(context, adapter.getAttenuationScale(), 0.0f, 1.f, 0.001f, "Attenuation Scale");
			adapter.setAttenuationScale(a);

			float b = GUISnippets::drawBarFloat(context, adapter.getBaseSize(), 0.0f, 8000.f, 1.f, "Base Size");
			adapter.setBaseSize(b);

			nk_tree_pop(context);
		}

	}

	std::string ShadowAdapterDebug::toString() const {
		return "Shadowmap adapter debug window";
	}

}