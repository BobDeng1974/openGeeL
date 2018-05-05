#include "transformation/transform.h"
#include "cameras/camera.h"
#include "cameras/perspectivecamera.h"
#include "lights/light.h"
#include "lights/directionallight.h"
#include "lights/spotlight.h"
#include "lights/pointlight.h"
#include "lights/lightmanager.h"
#include "shadowmapping/simpleshadowmap.h"
#include "renderscene.h"
#include "snippets/objectsnippets.h"
#include "guiwrapper.h"
#include "snippets/guisnippets.h"
#include "objectlister.h"

namespace geeL {

	ObjectLister::ObjectLister(Scene& scene) 
		: scene(scene) {

		scene.addListener(*this);

		scene.iterRenderObjects([&](MeshRenderer& renderer) {
			add(renderer);
		});

		LightManager& manager = scene.getLightmanager();
		manager.iterLights([this](Light& light) {
			ShadowMapSnippet* snippet = nullptr;

			//Hacky: Read specific shadow map type via casting
			Shadowmap* map = light.getShadowMap();
			if (map != nullptr) {
				SimpleShadowmap* simpleMap = dynamic_cast<SimpleShadowmap*>(map);

				if (simpleMap != nullptr)
					snippet = &createSnippet(*simpleMap);
			}

			if (snippet != nullptr)
				add(light, *snippet);
			else
				add(light);
		});
	}


	void ObjectLister::draw(GUIContext* context) {
		if (nk_tree_push(context, NK_TREE_NODE, "Cameras", NK_MINIMIZED)) {

			for (auto it = cameraSnippets.begin(); it != cameraSnippets.end(); it++) {
				GUISnippet& snippet = **it;
				snippet.draw(context);
			}

			nk_tree_pop(context);
		}
			
		if (nk_tree_push(context, NK_TREE_NODE, "Lights", NK_MAXIMIZED)) {

			for (auto it = lightSnippets.begin(); it != lightSnippets.end(); it++) {
				GUISnippet& snippet = **it;
				snippet.draw(context);
			}
				
			nk_tree_pop(context);
		}
			
		if (nk_tree_push(context, NK_TREE_NODE, "Objects", NK_MINIMIZED)) {
			for (auto it = meshSnippets.begin(); it != meshSnippets.end(); it++) {
				GUISnippet& snippet = *it->second;
				snippet.draw(context);
			}

			nk_tree_pop(context);
		}
	}

	std::string ObjectLister::toString() const {
		return "Object lister";
	}



	void ObjectLister::add(SceneCamera& cam) {
		CameraSnippet* snippet = new CameraSnippet(cam);
		cameraSnippets.push_back(std::unique_ptr<GUISnippet>(snippet));
	}

	void ObjectLister::add(PerspectiveCamera& cam) {
		PerspectiveCameraSnippet* snippet = new PerspectiveCameraSnippet(cam);
		cameraSnippets.push_back(std::unique_ptr<GUISnippet>(snippet));
	}

	void ObjectLister::add(MeshRenderer & mesh) {
		SingleMeshRendererSnippet* snippet = new SingleMeshRendererSnippet(mesh);
		meshSnippets[&mesh] = std::unique_ptr<GUISnippet>(snippet);
	}

	void ObjectLister::add(Light& light) {
		LightSnippet* snippet = new LightSnippet(light);
		lightSnippets.push_back(std::unique_ptr<GUISnippet>(snippet));
	}

	void ObjectLister::add(Light& light, ShadowMapSnippet& mapSnippet) {
		LightSnippet* snippet = new LightSnippet(light, mapSnippet);
		lightSnippets.push_back(std::unique_ptr<GUISnippet>(snippet));
	}

	ShadowMapSnippet& ObjectLister::createSnippet(SimpleShadowmap& map) {
		ShadowMapSnippet* snippet = new ShadowMapSnippet(map);
		otherSnippets.push_back(std::unique_ptr<GUISnippet>(snippet));

		return *snippet;
	}

	void ObjectLister::onAdd(MeshRenderer& renderer) {
		add(renderer);
	}

	void ObjectLister::onRemove(std::shared_ptr<MeshRenderer> renderer) {
		MeshRenderer* r = renderer.get();
		if(r) meshSnippets.erase(r);
	}

}