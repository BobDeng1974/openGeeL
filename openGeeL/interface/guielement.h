#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include "../interface/guiwrapper.h"

namespace geeL {

	typedef nk_context GUIContext;

	class GUIElement {

	public:
		virtual void draw(GUIContext* context) = 0;

	};
}

#endif
