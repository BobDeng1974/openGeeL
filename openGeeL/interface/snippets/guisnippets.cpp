#include "../../renderer/materials/defaultmaterial.h"
#include "../../renderer/materials/genericmaterial.h"
#include "../../renderer/lighting/light.h"
#include "../../renderer/transformation/transform.h"
#include "../guiwrapper.h"
#include "guisnippets.h"
#include <iostream>

namespace geeL {


	int GUISnippets::drawBarInteger(GUIContext* context, int value, int min, int max, int step, std::string name) {
		int val = value;

		nk_layout_row_dynamic(context, 30, 3);
		nk_label(context, name.c_str(), NK_TEXT_CENTERED);
		nk_slider_int(context, min, &val, max, step);

		std::string valName = std::to_string(val);
		nk_label(context, valName.c_str(), NK_TEXT_CENTERED);

		return val;
	}

	float GUISnippets::drawBarFloat(GUIContext* context, float value, float min, float max, float step, std::string name) {
		float val = value;

		nk_layout_row_dynamic(context, 30, 3);
		nk_label(context, name.c_str(), NK_TEXT_CENTERED);
		nk_slider_float(context, min, &val, max, step);

		std::string valName = std::to_string(val);
		valName = valName.substr(0, 5);
		nk_label(context, valName.c_str(), NK_TEXT_CENTERED);

		return val;
	}


	void GUISnippets::drawVector(GUIContext* context, glm::vec3& vector, std::string prefix, float border, float step) {

		nk_layout_row_dynamic(context, 30, 3);

		std::string x = prefix + " X: ";
		std::string y = prefix + " Y: ";
		std::string z = prefix + " Z: ";

		nk_property_float(context, x.c_str(), -border, &vector.x, border, 1, step);
		nk_property_float(context, y.c_str(), -border, &vector.y, border, 1, step);
		nk_property_float(context, z.c_str(), -border, &vector.z, border, 1, step);
	}

	glm::vec3 GUISnippets::drawVector2(GUIContext* context, const glm::vec3& vector, std::string prefix, float border, float step) {

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

			glm::vec3 pos = drawVector2(context, transform.getPosition(), "P", 100.f, 0.1f);
			transform.setPosition(pos);

			glm::vec3 rot = drawVector2(context, transform.getEulerAngles(), "R", 360.f, 0.1f);
			transform.setEulerAngles(rot);

			glm::vec3 scale = drawVector2(context, transform.getScaling(), "S", 100.f, 0.1f);
			transform.setScaling(scale);

			nk_combo_end(context);
		}
	}

	void GUISnippets::drawTransformTree(GUIContext* context, Transform& transform) {

		std::string name = "Transform(" + std::to_string(transform.getID()) + "):";
		if (nk_tree_push(context, NK_TREE_NODE, name.c_str(), NK_MINIMIZED)) {

			glm::vec3 position = drawVector2(context, transform.getPosition(), "P", 100.f, 0.1f);
			transform.setPosition(position);

			glm::vec3 rotation = drawVector2(context, transform.getEulerAngles(), "R", 100.f, 0.1f);
			transform.setEulerAngles(rotation);

			glm::vec3 scaling = drawVector2(context, transform.getScaling(), "S", 100.f, 0.1f);
			transform.setScaling(scaling);

			nk_tree_pop(context);
		}
	}

	void GUISnippets::drawColor(GUIContext* context, glm::vec3& color) {

		
		nk_color c = nk_rgb(color.r, color.g, color.b);
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

	void GUISnippets::drawMaterial(GUIContext* context, GenericMaterialContainer* material) {
		nk_layout_row_dynamic(context, 30, 1);
		nk_label(context, "Generic material not supported in GUI", NK_TEXT_LEFT);
	}

	void GUISnippets::drawMaterial(GUIContext* context, DefaultMaterialContainer* material) {
		float roughness = drawBarFloat(context, material->getRoughness(), 0.f, 1.0f, 0.001f, "Roughness");
		material->setRoughness(roughness);

		float metallic = drawBarFloat(context, material->getMetallic(), 0.f, 1.0f, 0.001f, "Metallic");
		material->setMetallic(metallic);

		nk_layout_row_dynamic(context, 20, 1);
		glm::vec3 color = material->getColor();
		color = glm::vec3(color.r * 255.f, color.g * 255.f, color.b * 255.f);

		drawColor(context, color);
		material->setColor(glm::vec3(color.r / 255.f, color.g / 255.f, color.b / 255.f));
	}

}