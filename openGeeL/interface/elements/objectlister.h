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
	class MeshRenderer;
	class Light;
	class Scene;
	class ShadowMapSnippet;
	class SimpleShadowMap;

	class ObjectLister : public GUISnippet, public DataEventListener<MeshRenderer> {

	public:
		ObjectLister(Scene& scene);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

		//void add(SceneObject& obj);
		void add(SceneCamera& cam);
		void add(PerspectiveCamera& cam);
		void add(MeshRenderer& mesh);
		void add(Light& light);
		void add(Light& light, ShadowMapSnippet& mapSnippet);

		ShadowMapSnippet& createSnippet(SimpleShadowMap& map);

		virtual void onAdd(MeshRenderer& renderer);
		virtual void onRemove(MeshRenderer& renderer);

	private:
		std::list<std::unique_ptr<GUISnippet>> lightSnippets;
		std::map<MeshRenderer*, std::unique_ptr<GUISnippet>> objectSnippets;
		std::list<std::unique_ptr<GUISnippet>> cameraSnippets;
		std::list<std::unique_ptr<GUISnippet>> otherSnippets;
		Scene& scene;

	};
}

#endif
