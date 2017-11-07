#include <vector>
#include <iostream>
#include "renderer/deferredrenderer.h"
#include "texturing/texture.h"
#include "utility/rendertime.h"
#include "guiwrapper.h"
#include "systeminformation.h"

namespace geeL {

	int modulo(int a, int b) {
		int result = a % b;
		return result >= 0 ? result : result + b;
	}


	SystemInformation::SystemInformation(RenderWindow& window, DeferredRenderer& renderer,
		float x, float y, float width, float height) 
			: GUIElement(window, x, y, width, height), renderer(renderer) {}


	void SystemInformation::draw(GUIContext* context) {

		if (nk_begin(context, "System Information", nk_rect(x, y, width, height),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

			drawTime(context, "Render Time:");

			nk_layout_row_dynamic(context, 30, 1);
			int reset = nk_button_label(context, "Reset");
			nk_layout_row_dynamic(context, 30, 2);
			int back = -nk_button_label(context, "<");
			int forward = nk_button_label(context, ">");

			if (reset != 0)
				renderer.setScreenImage();
			else if (back != 0 || forward != 0) {
				std::vector<const Texture*> textures(std::move(renderer.getBuffers()));
				unsigned int size = unsigned int(textures.size());

				if (size != 0) {
					position = modulo((position + back + forward), size);
					renderer.setScreenImage(textures[position]);
				}
			}
		}

		nk_end(context);
	}

	void SystemInformation::drawTime(GUIContext* context, std::string name) {

		nk_layout_row_dynamic(context, 30, 2);
		nk_label(context, name.c_str(), NK_TEXT_RIGHT);

		std::string t = std::to_string(RenderTime::deltaTime()) + " ms";
		nk_label(context, t.c_str(), NK_TEXT_LEFT);
	}

}