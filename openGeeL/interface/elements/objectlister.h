#ifndef OBJECTLISTER_H
#define OBJECTLISTER_H

#include <list>
#include "guielement.h"

namespace geeL {

	class GUISnippet;
	class SceneObject;
	class SceneCamera;
	class PerspectiveCamera;
	class MeshRenderer;
	class Light;
	class Scene;
	class ShadowMapSnippet;
	class SimpleShadowMap;

	class ObjectLister : public GUIElement {

	public:
		ObjectLister(Scene& scene, RenderWindow& window, 
			float x = 0.1f, float y = 0.1f, float width = 0.2f, float height = 0.2f);
		~ObjectLister();

		virtual void draw(GUIContext* context);

		void add(SceneObject& obj);
		void add(SceneCamera& cam);
		void add(PerspectiveCamera& cam);
		void add(MeshRenderer& mesh);
		void add(Light& light);
		void add(Light& light, ShadowMapSnippet& mapSnippet);

		ShadowMapSnippet& createSnippet(SimpleShadowMap& map);

	private:
		std::list<GUISnippet*> lightSnippets;
		std::list<GUISnippet*> objectSnippets;
		std::list<GUISnippet*> cameraSnippets;
		std::list<GUISnippet*> otherSnippets;
		Scene& scene;

	};
}

#endif
