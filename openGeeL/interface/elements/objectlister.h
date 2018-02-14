#ifndef OBJECTLISTER_H
#define OBJECTLISTER_H

#include <list>
#include <map>
#include <memory>
#include "snippets/guisnippets.h"
#include "utility/listener.h"

namespace geeL {

	class SceneObject;
	class SceneCamera;
	class PerspectiveCamera;
	class Light;
	class Scene;
	class ShadowMapSnippet;
	class SimpleShadowmap;
	class SingleMeshRenderer;

	class ObjectLister : public GUISnippet, public DataEventListener<SingleMeshRenderer> {

	public:
		ObjectLister(Scene& scene);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

		//void add(SceneObject& obj);
		void add(SceneCamera& cam);
		void add(PerspectiveCamera& cam);
		void add(SingleMeshRenderer& mesh);
		void add(Light& light);
		void add(Light& light, ShadowMapSnippet& mapSnippet);

		ShadowMapSnippet& createSnippet(SimpleShadowmap& map);

		virtual void onAdd(SingleMeshRenderer& renderer);
		virtual void onRemove(std::shared_ptr<SingleMeshRenderer> renderer);

	private:
		std::list<std::unique_ptr<GUISnippet>> lightSnippets;
		std::map<SingleMeshRenderer*, std::unique_ptr<GUISnippet>> meshSnippets;
		std::list<std::unique_ptr<GUISnippet>> cameraSnippets;
		std::list<std::unique_ptr<GUISnippet>> otherSnippets;
		Scene& scene;

	};
}

#endif
