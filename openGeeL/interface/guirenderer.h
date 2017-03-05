#ifndef GUIRENDERER_H
#define GUIRENDERER_H

#include <list>
#include "../renderer/renderer.h"

struct nk_context;
typedef nk_context GUIContext;

namespace geeL {

	class GUIElement;
	class RenderContext;
	class RenderWindow;

	class GUIRenderer : public Drawer {

	public:
		GUIRenderer(const RenderWindow& window, RenderContext& context);

		virtual void draw();
		void addElement(GUIElement& element);

	private:
		std::list<GUIElement*> elements;
		GUIContext* guiContext;
		RenderContext* renderContext;

		void init(const RenderWindow& window);

	};
}

#endif

