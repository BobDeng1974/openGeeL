#include "../renderer/lighting/light.h"
#include "../renderer/transformation/transform.h"
#include "guiwrapper.h"
#include "guisnippets.h"

namespace geeL {


	void GUISnippets::drawVector(GUIContext* context, glm::vec3& vector, std::string prefix, float border, float step) {

		nk_layout_row_dynamic(context, 30, 3);

		std::string x = prefix + " X: ";
		std::string y = prefix + " Y: ";
		std::string z = prefix + " Z: ";

		nk_property_float(context, x.c_str(), -border, &vector.x, border, 1, step);
		nk_property_float(context, y.c_str(), -border, &vector.y, border, 1, step);
		nk_property_float(context, z.c_str(), -border, &vector.z, border, 1, step);
	}

	glm::vec3 GUISnippets::drawVector2(GUIContext* context, glm::vec3& vector, std::string prefix, float border, float step) {

		nk_layout_row_dynamic(context, 30, 3);

		std::string x = prefix + " X: ";
		std::string y = prefix + " Y: ";
		std::string z = prefix + " Z: ";

		glm::vec3 vec = vector;

		nk_property_float(context, x.c_str(), -border, &vec.x, border, 1, step);
		nk_property_float(context, y.c_str(), -border, &vec.y, border, 1, step);
		nk_property_float(context, z.c_str(), -border, &vec.z, border, 1, step);

		return vec;
	}

	void GUISnippets::drawTransform(GUIContext* context, Transform& transform) {

		std::string name = "Transform(" + std::to_string(transform.getID()) + "):";
		if (nk_combo_begin_label(context, name.c_str(), nk_vec2(nk_widget_width(context), 400))) {

			glm::vec3 pos = drawVector2(context, transform.position, "P", 100.f, 0.1f);
			transform.setPosition(pos);

			glm::vec3 rot = drawVector2(context, transform.rotation, "R", 360.f, 0.1f);
			transform.setRotation(rot);

			//glm::vec3 scale = drawVector2(context, transform.scaling, "S", 100.f, 0.1f);
			//transform.setScale(scale);

			nk_combo_end(context);
		}
	}

	void GUISnippets::drawTransformTree(GUIContext* context, Transform& transform) {

		std::string name = "Transform(" + std::to_string(transform.getID()) + "):";
		if (nk_tree_push(context, NK_TREE_NODE, name.c_str(), NK_MINIMIZED)) {

			drawVector(context, transform.position, "P", 100.f, 0.1f);
			drawVector(context, transform.rotation, "R", 100.f, 0.1f);
			drawVector(context, transform.scaling, "S", 100.f, 0.1f);
			nk_tree_pop(context);
		}
	}

	void GUISnippets::drawColor(GUIContext* context, glm::vec3& color) {

		nk_color c = nk_rgb(color.r, color.g, color.b);

		nk_layout_row_dynamic(context, 25, 1);
		if (nk_combo_begin_color(context, c, nk_vec2(nk_widget_width(context), 400))) {
			nk_layout_row_dynamic(context, 120, 1);
			c = nk_color_picker(context, c, NK_RGB);
			nk_layout_row_dynamic(context, 25, 1);
			color.r = (nk_byte)nk_propertyi(context, "#R:", 0, c.r, 255, 1, 1);
			color.g = (nk_byte)nk_propertyi(context, "#G:", 0, c.g, 255, 1, 1);
			color.b = (nk_byte)nk_propertyi(context, "#B:", 0, c.b, 255, 1, 1);
			nk_combo_end(context);
		}
	}

	void GUISnippets::drawLight(GUIContext* context, Light& light) {

		std::string id = "Light #" + std::to_string(light.transform.getID());

		if (nk_tree_push(context, NK_TREE_NODE, id.c_str(), NK_MINIMIZED)) {

			drawColor(context, light.diffuse);
			drawTransform(context, light.transform);

			nk_layout_row_dynamic(context, 30, 2);
			nk_label(context, "Shadow Bias", NK_TEXT_LEFT);
			nk_slider_float(context, 0.f, &light.dynamicBias, 0.01f, 0.0001f);

			nk_tree_pop(context);
		}
	}

	
}