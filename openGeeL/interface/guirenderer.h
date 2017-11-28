#ifndef GUIRENDERER_H
#define GUIRENDERER_H

#include <list>
#include <memory>
#include "renderer/renderer.h"

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

		template<typename T>
		void addElement(std::unique_ptr<T> element);

	private:
		std::list<std::pair<bool, GUIElement*>> elements;
		GUIContext* guiContext;
		RenderWindow& window;
		DeferredRenderer& renderer;
		RenderContext* renderContext;

		void init(const RenderWindow& window);

	};


	template<typename T>
	inline void GUIRenderer::addElement(std::unique_ptr<T> element) {
		GUIElement* e = element.release();
		elements.push_back(std::pair<bool, GUIElement*>(true, e));
	}

}

#endif

