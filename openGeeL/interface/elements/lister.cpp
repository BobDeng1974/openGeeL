#include "guiwrapper.h"
#include "snippets/guisnippets.h"
#include "lister.h"

namespace geeL {

	GUILister::GUILister(RenderWindow & window, 
		float x, float y, 
		float width, float height, 
		GUISnippet & element)
			: GUIElement(window, x, y, width, height) {
	
		add(element);
	}

	GUILister::GUILister(RenderWindow & window, 
		float x, float y, 
		float width, float height)
			: GUIElement(window, x, y, width, height) {}


	void GUILister::draw(GUIContext* context) {
		if (nk_begin(context, "Scene", nk_rect(x, y, width, height),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

			for (auto it(elements.begin()); it != elements.end(); it++) {
				GUISnippet& list = **it;
				list.draw(context);

				nk_layout_row_dynamic(context, 7, 1);
			}
		}

		nk_end(context);
	}

	void GUILister::add(GUISnippet& element) {
		elements.push_back(&element);
	}

}