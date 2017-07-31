#ifndef SYSTEMINFORMATION_H
#define SYSTEMINFORMATION_H

#include <string>
#include "guielement.h"

namespace geeL {

	class SystemInformation : public GUIElement {

	public:
		SystemInformation(RenderWindow& window, 
			float x = 0.1f, float y = 0.1f, float width = 0.2f, float height = 0.2f);

		virtual void draw(GUIContext* context);

	private:

		void drawTime(GUIContext* context, std::string name);

	};
}

#endif