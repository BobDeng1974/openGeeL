#ifndef GUITESTELEMENT_H
#define GUITESTELEMENT_H

#include <stdio.h>
#include "../guielement.h"

namespace geeL {

	class GUITestElement : public GUIElement {

	public:
		GUITestElement(std::string name) {
			this->name = name;
		}

		virtual void draw(GUIContext* context) {
			if (nk_begin(context, name.c_str(), nk_rect(50, 50, 230, 250),
				NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
				NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
			{
				enum { EASY, HARD };
				static int op = EASY;
				static int property = 20;
				nk_layout_row_static(context, 30, 80, 1);
				if (nk_button_label(context, "button"))
					fprintf(stdout, "button pressed\n");

				nk_layout_row_dynamic(context, 30, 2);
				if (nk_option_label(context, "easy", op == EASY)) op = EASY;
				if (nk_option_label(context, "hard", op == HARD)) op = HARD;

				nk_layout_row_dynamic(context, 25, 1);
				nk_property_int(context, "Compression:", 0, &property, 100, 10, 1);

				nk_layout_row_dynamic(context, 20, 1);
				nk_label(context, "background:", NK_TEXT_LEFT);
				nk_layout_row_dynamic(context, 25, 1);
				if (nk_combo_begin_color(context, background, nk_vec2(nk_widget_width(context), 400))) {
					nk_layout_row_dynamic(context, 120, 1);
					background = nk_color_picker(context, background, NK_RGBA);
					nk_layout_row_dynamic(context, 25, 1);
					background.r = (nk_byte)nk_propertyi(context, "#R:", 0, background.r, 255, 1, 1);
					background.g = (nk_byte)nk_propertyi(context, "#G:", 0, background.g, 255, 1, 1);
					background.b = (nk_byte)nk_propertyi(context, "#B:", 0, background.b, 255, 1, 1);
					background.a = (nk_byte)nk_propertyi(context, "#A:", 0, background.a, 255, 1, 1);
					nk_combo_end(context);
				}
			}

			nk_end(context);
		}


	private:
		std::string name;
		struct nk_color background;

	};
}

#endif

