
#include "../../renderer/utility/rendertime.h"
#include "../guiwrapper.h"
#include "systeminformation.h"

namespace geeL {

	SystemInformation::SystemInformation(const RenderTime& time, RenderWindow& window, 
		float x, float y, float width, float height) 
			: GUIElement(window, x, y, width, height), time(time) {}


	void SystemInformation::draw(GUIContext* context) {

		if (nk_begin(context, "System Information", nk_rect(x, y, width, height),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

			drawTime(context, RenderPass::Geometry, "Geometry Pass: ");
			drawTime(context, RenderPass::Shadow, "Shadow Pass: ");
			drawTime(context, RenderPass::SSAO, "SSAO Pass: ");
			drawTime(context, RenderPass::Lighting, "Lighting Pass: ");
			drawTime(context, RenderPass::PostProcessing, "Processing Pass: ");
			drawTime(context, RenderPass::GUI, "GUI Pass: ");
		}

		nk_end(context);
	}

	void SystemInformation::drawTime(GUIContext* context, RenderPass pass, std::string name) {

		nk_layout_row_dynamic(context, 30, 2);
		nk_label(context, name.c_str(), NK_TEXT_RIGHT);

		std::string t = std::to_string(time.getTime(pass)) + " ms";
		nk_label(context, t.c_str(), NK_TEXT_LEFT);
	}

}