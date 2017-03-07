#include "../../renderer/window.h"
#include "guielement.h"

namespace geeL {

	GUIElement::GUIElement(RenderWindow& window, float x, float y, float width, float height) 
		: x(window.width *x), y(window.height * y), width(window.width * width), height(window.height * height) {}

}