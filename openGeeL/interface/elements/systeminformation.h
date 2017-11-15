#ifndef SYSTEMINFORMATION_H
#define SYSTEMINFORMATION_H

#include <string>
#include <vector>
#include "guielement.h"

namespace geeL {

	class DeferredRenderer;

	class SystemInformation : public GUIElement {

	public:
		SystemInformation(RenderWindow& window, DeferredRenderer& renderer,
			float x = 0.1f, float y = 0.1f, float width = 0.2f, float height = 0.2f);

		virtual void draw(GUIContext* context);

	private:
		int position = -1;
		DeferredRenderer& renderer;

		void drawTime(GUIContext* context, std::string name);
		std::vector<const Texture*> getBuffers();

	};
}

#endif