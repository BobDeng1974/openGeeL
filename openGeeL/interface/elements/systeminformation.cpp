
#include "utility/rendertime.h"
#include "guiwrapper.h"
#include "systeminformation.h"

namespace geeL {

	SystemInformation::SystemInformation(RenderWindow& window, 
		float x, float y, float width, float height) 
			: GUIElement(window, x, y, width, height) {}


	void SystemInformation::draw(GUIContext* context) {

		if (nk_begin(context, "System Information", nk_rect(x, y, width, height),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

			drawTime(context, "Render Pass: ");
		}

		nk_end(context);
	}

	void SystemInformation::drawTime(GUIContext* context, std::string name) {

		nk_layout_row_dynamic(context, 30, 2);
		nk_label(context, name.c_str(), NK_TEXT_RIGHT);

		std::string t = std::to_string(RenderTime::deltaTime) + " ms";
		nk_label(context, t.c_str(), NK_TEXT_LEFT);
	}

}