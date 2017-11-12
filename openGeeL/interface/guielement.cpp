#include "window.h"
#include "guielement.h"

namespace geeL {

	GUIElement::GUIElement(RenderWindow& window, 
		float x, 
		float y, 
		float width, 
		float height) 
			: x(window.getWidth() *x)
			, y(window.getHeight() * y)
			, width(window.getWidth() * width)
			, height(window.getHeight() * height) {}

}