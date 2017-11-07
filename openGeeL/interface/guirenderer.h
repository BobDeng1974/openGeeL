#ifndef GUIRENDERER_H
#define GUIRENDERER_H

#include <list>
#include "renderer.h"

struct nk_context;
typedef nk_context GUIContext;

namespace geeL {

	class DeferredRenderer;
	class GUIElement;
	class RenderContext;
	class RenderWindow;

	class GUIRenderer : public Drawer {

	public:
		GUIRenderer(RenderWindow& window, RenderContext& context, DeferredRenderer& renderer);
		virtual ~GUIRenderer();

		virtual void draw();
		void addElement(GUIElement& element);
		void addSystemInformation(float x, float y, float width, float height);

	private:
		std::list<std::pair<bool, GUIElement*>> elements;
		GUIContext* guiContext;
		RenderWindow& window;
		DeferredRenderer& renderer;
		RenderContext* renderContext;

		void init(const RenderWindow& window);

	};
}

#endif

