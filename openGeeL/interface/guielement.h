#ifndef GUIELEMENT_H
#define GUIELEMENT_H

struct nk_context;

namespace geeL {

	typedef nk_context GUIContext;

	//Object that represents one GUI element on the canvas
	class GUIElement {

	public:
		virtual void draw(GUIContext* context) = 0;

	};
}

#endif
