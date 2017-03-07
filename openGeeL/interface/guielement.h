#ifndef GUIELEMENT_H
#define GUIELEMENT_H

struct nk_context;

namespace geeL {

	typedef nk_context GUIContext;

	class RenderWindow;

	//Object that represents one GUI element on the canvas
	class GUIElement {

	public:
		GUIElement(RenderWindow& window, float x = 0.1f, float y = 0.1f, float width = 0.2f, float height = 0.2f);

		virtual void draw(GUIContext* context) = 0;

	protected:
		float x, y, width, height;

	};
}

#endif
