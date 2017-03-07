#ifndef SYSTEMINFORMATION_H
#define SYSTEMINFORMATION_H

#include <string>
#include "../guielement.h"

namespace geeL {

	class RenderTime;

	class SystemInformation : public GUIElement {

	public:
		SystemInformation(const RenderTime& time, RenderWindow& window, 
			float x = 0.1f, float y = 0.1f, float width = 0.2f, float height = 0.2f);

		virtual void draw(GUIContext* context);

	private:
		const RenderTime& time;

		void drawTime(GUIContext* context, RenderPass pass, std::string name);

	};
}

#endif