#include <cmath>
#include <iostream>
#include "materials/defaultmaterial.h"
#include "materials/genericmaterial.h"
#include "lights/light.h"
#include "transformation/transform.h"
#include "guiwrapper.h"
#include "guisnippets.h"

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

	int GUISnippets::drawBarIntegerLogarithmic(GUIContext * context, int value, int min, int max, int step, std::string name) {
		int val  = log(1 + value);
		int maxi = log(max);

		nk_layout_row_dynamic(context, 30, 3);
		nk_label(context, name.c_str(), NK_TEXT_CENTERED);
		nk_slider_int(context, min, &val, max, step);

		val = exp(val) - 1;

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

	float GUISnippets::drawBarFloatLogarithmic(GUIContext* context, float value, float min, float max, float step, std::string name) {
		float val  = log(1.f + value);
		float maxi = log(max);

		nk_layout_row_dynamic(context, 30, 3);
		nk_label(context, name.c_str(), NK_TEXT_CENTERED);
		nk_slider_float(context, min, &val, maxi, step);

		val = exp(val) - 1.f;

		std::string valName = std::to_string(val);
		valName = valName.substr(0, 5);
		nk_label(context, valName.c_str(), NK_TEXT_CENTERED);

		return val;
	}

	glm::vec2 GUISnippets::drawVector(GUIContext* context, const glm::vec2& vector, std::string prefix, float border, float step) {

		nk_layout_row_dynamic(context, 30, 3);

		std::string x = prefix + " X: ";
		std::string y = prefix + " Y: ";

		glm::vec2 vec = vector;
		nk_property_float(context, x.c_str(), -border, &vec.x, border, 1, step);
		nk_property_float(context, y.c_str(), -border, &vec.y, border, 1, step);

		return vec;
	}

	glm::vec3 GUISnippets::drawVector(GUIContext* context, const glm::vec3& vector, std::string prefix, float border, float step) {

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

		nk_layout_row_dynamic(context, 20, 1);
		std::string name = "Transform(" + std::to_string(transform.getID()) + "):";
		if (nk_combo_begin_label(context, name.c_str(), nk_vec2(nk_widget_width(context), 400))) {

			glm::vec3 pos = drawVector(context, transform.getPosition(), "P", 1000.f, 0.1f);
			transform.setPosition(pos);

			glm::vec3 rot = drawVector(context, transform.getEulerAngles(), "R", 3600.f, 0.1f);
			rot = glm::vec3(fmod(rot.x, 360.f), fmod(rot.y, 360.f), fmod(rot.z, 360.f));
			transform.setEulerAngles(rot);

			glm::vec3 scale = drawVector(context, transform.getScaling(), "S", 100.f, 0.1f);
			transform.setScaling(scale);

			nk_combo_end(context);
		}
	}

	void GUISnippets::drawTransformTree(GUIContext* context, Transform& transform) {

		std::string name = "Transform(" + std::to_string(transform.getID()) + "):";
		if (nk_tree_push(context, NK_TREE_NODE, name.c_str(), NK_MINIMIZED)) {

			glm::vec3 position = drawVector(context, transform.getPosition(), "P", 100.f, 0.1f);
			transform.setPosition(position);

			glm::vec3 rotation = drawVector(context, transform.getEulerAngles(), "R", 100.f, 0.1f);
			transform.setEulerAngles(rotation);

			glm::vec3 scaling = drawVector(context, transform.getScaling(), "S", 100.f, 0.1f);
			transform.setScaling(scaling);

			nk_tree_pop(context);
		}
	}

	void GUISnippets::drawColor(GUIContext* context, glm::vec3& color) {

		nk_color c = nk_rgb(color.r * 256.f, color.g * 256.f, color.b * 256.f);
		if (nk_combo_begin_color(context, c, nk_vec2(nk_widget_width(context), 400))) {
			nk_layout_row_dynamic(context, 120, 1);
			c = nk_color_picker(context, c, NK_RGB);
			nk_layout_row_dynamic(context, 25, 1);

			color.r = (float) nk_propertyi(context, "#R:", 0, c.r, 255, 1, 1) / 256.f;
			color.g = (float) nk_propertyi(context, "#G:", 0, c.g, 255, 1, 1) / 256.f;
			color.b = (float) nk_propertyi(context, "#B:", 0, c.b, 255, 1, 1) / 256.f;

			nk_combo_end(context);
		}
	}

	void GUISnippets::drawMaterial(GUIContext* context, GenericMaterialContainer* material) {
		nk_layout_row_dynamic(context, 30, 1);
		nk_label(context, "Generic material not supported in GUI", NK_TEXT_LEFT);
	}

	void GUISnippets::drawMaterial(GUIContext* context, DefaultMaterialContainer* material) {
		float transparency = drawBarFloat(context, material->getTransparency(), 0.f, 1.f, 0.001f, "Transparency");
		material->setTransparency(transparency);

		float roughness = drawBarFloat(context, material->getRoughness(), 0.f, 1.f, 0.001f, "Roughness");
		material->setRoughness(roughness);

		float metallic = drawBarFloat(context, material->getMetallic(), 0.f, 1.f, 0.001f, "Metallic");
		material->setMetallic(metallic);

		nk_layout_row_dynamic(context, 20, 1);
		glm::vec3 color = material->getColor();

		drawColor(context, color);
		material->setColor(color);
	}

	void GUISnippets::drawTreeNode(GUIContext* context, const std::string& name, 
		bool minimized, std::function<void(GUIContext*)> function) {
		
		if (nk_tree_push(context, NK_TREE_NODE, name.c_str(), minimized ? NK_MINIMIZED : NK_MAXIMIZED)) {
			function(context);
			nk_tree_pop(context);
		}
	}

}