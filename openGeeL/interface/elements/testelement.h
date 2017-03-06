#ifndef GUITESTELEMENT_H
#define GUITESTELEMENT_H

#include <stdio.h>
#include <vec3.hpp>
#include "../../renderer/lighting/light.h"
#include "../../renderer/transformation/transform.h"
#include "../guielement.h"
#include "../interface/guiwrapper.h"
#include "../guisnippets.h"

namespace geeL {

	class GUITestElement : public GUIElement {

	public:
		GUITestElement(std::string name, Light& light) : light(light) {
			this->name = name;
		}


		glm::vec3 position = glm::vec3(0.f, 1.f, 0.f);

		virtual void draw(GUIContext* context) {
			if (nk_begin(context, name.c_str(), nk_rect(100, 50, 230, 250),
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

				
				GUISnippets::drawLight(context, light);

			}

			nk_end(context);
		}


	private:
		Light& light;
		std::string name;
		struct nk_color background;

	};
}

#endif

