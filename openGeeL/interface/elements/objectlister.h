#ifndef OBJECTLISTER_H
#define OBJECTLISTER_H

#include <list>
#include "../guielement.h"

namespace geeL {

	class GUISnippet;
	class SceneObject;
	class MeshRenderer;
	class Light;
	class RenderScene;

	class ObjectLister : public GUIElement {

	public:
		ObjectLister(RenderScene& scene, RenderWindow& window, 
			float x = 0.1f, float y = 0.1f, float width = 0.2f, float height = 0.2f);
		~ObjectLister();

		virtual void draw(GUIContext* context);

		void add(SceneObject& obj);
		void add(MeshRenderer& mesh);
		void add(Light& light);

	private:
		std::list<GUISnippet*> lightSnippets;
		std::list<GUISnippet*> objectSnippets;
		RenderScene& scene;

	};
}

#endif
