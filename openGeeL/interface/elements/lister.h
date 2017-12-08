#ifndef GUILISTER_H
#define GUILISTER_H

#include <list>
#include "guielement.h"

namespace geeL {

	class GUISnippet;
	class RenderWindow;


	class GUILister : public GUIElement {

	public:
		template<typename ...GUISnippets>
		GUILister(RenderWindow& window, float x, float y, float width, float height, GUISnippets& ...elements);
		GUILister(RenderWindow& window, float x, float y, float width, float height, GUISnippet& element);
		GUILister(RenderWindow& window, float x, float y, float width, float height);

		template<typename ...GUISnippets>
		void add(GUISnippet& element, GUISnippets& ...elements);
		void add(GUISnippet& element);

		virtual void draw(GUIContext* context);

	private:
		std::list<GUISnippet*> elements;

	};


	template<typename ...GUISnippets>
	inline GUILister::GUILister(RenderWindow& window, 
		float x, float y, 
		float width, float height, 
		GUISnippets& ...elements)
			: GUIElement(window, x, y, width, height) {

		add(elements...);
	}

	template<typename ...GUISnippets>
	inline void GUILister::add(GUISnippet& element, GUISnippets& ...elements) {
		add(element);
		add(elements...);
	}

}

#endif