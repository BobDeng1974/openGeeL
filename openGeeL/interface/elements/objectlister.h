#ifndef OBJECTLISTER_H
#define OBJECTLISTER_H

#include <list>
#include "snippets/guisnippets.h"

namespace geeL {

	class SceneObject;
	class SceneCamera;
	class PerspectiveCamera;
	class MeshRenderer;
	class Light;
	class Scene;
	class ShadowMapSnippet;
	class SimpleShadowMap;

	class ObjectLister : public GUISnippet {

	public:
		ObjectLister(Scene& scene);
		virtual ~ObjectLister();

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

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
