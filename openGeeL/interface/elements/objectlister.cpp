#include "../../renderer/transformation/transform.h"
#include "../../renderer/meshes/meshrenderer.h"
#include "../../renderer/cameras/camera.h"
#include "../../renderer/cameras/perspectivecamera.h"
#include "../../renderer/lighting/light.h"
#include "../../renderer/lighting/directionallight.h"
#include "../../renderer/lighting/spotlight.h"
#include "../../renderer/lighting/pointlight.h"
#include "../../renderer/lighting/lightmanager.h"
#include "../../renderer/scene.h"
#include "../snippets/objectsnippets.h"
#include "../guiwrapper.h"
#include "../snippets/guisnippets.h"
#include "objectlister.h"

namespace geeL {

	ObjectLister::ObjectLister(RenderScene& scene, RenderWindow& window, 
		float x, float y, float width, float height) 
			: GUIElement(window, x, y, width, height), scene(scene) {
	
		for (auto it = scene.renderObjectsBegin(); it != scene.renderObjectsEnd(); it++) {
			MeshRenderer* renderer = *it;
			add(*renderer);
		}

		LightManager& manager = scene.lightManager;
		for (auto it = manager.directionalLightsBegin(); it != manager.directionalLightsEnd(); it++) {
			Light& light = **it;
			add(light);
		}

		for (auto it = manager.spotLightsBegin(); it != manager.spotLightsEnd(); it++) {
			Light& light = **it;
			add(light);
		}

		for (auto it = manager.pointLightsBegin(); it != manager.pointLightsEnd(); it++) {
			Light& light = **it;
			add(light);
		}
	}

	ObjectLister::~ObjectLister() {
		for (auto it = lightSnippets.begin(); it != lightSnippets.end(); it++)
			delete *it;
		
		for (auto it = objectSnippets.begin(); it != objectSnippets.end(); it++)
			delete *it;

		for (auto it = cameraSnippets.begin(); it != cameraSnippets.end(); it++)
			delete *it;
	}


	void ObjectLister::draw(GUIContext* context) {

		if (nk_begin(context, "Scene", nk_rect(x, y, width, height),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

			if (nk_tree_push(context, NK_TREE_NODE, "Cameras", NK_MAXIMIZED)) {

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
			
			if (nk_tree_push(context, NK_TREE_NODE, "Objects", NK_MAXIMIZED)) {
				for (auto it = objectSnippets.begin(); it != objectSnippets.end(); it++) {
					GUISnippet& snippet = **it;
					snippet.draw(context);
				}

				nk_tree_pop(context);
			}

		}

		nk_end(context);
	}


	void ObjectLister::add(SceneObject& obj) {
		SceneObjectSnippet* snippet = new SceneObjectSnippet(obj);
		objectSnippets.push_back(snippet);
	}

	void ObjectLister::add(Camera& cam) {
		CameraSnippet* snippet = new CameraSnippet(cam);
		cameraSnippets.push_back(snippet);
	}

	void ObjectLister::add(PerspectiveCamera& cam) {
		PerspectiveCameraSnippet* snippet = new PerspectiveCameraSnippet(cam);
		cameraSnippets.push_back(snippet);
	}

	void ObjectLister::add(MeshRenderer& mesh) {
		MeshRendererSnippet* snippet = new MeshRendererSnippet(mesh);
		objectSnippets.push_back(snippet);
	}

	void ObjectLister::add(Light& light) {
		LightSnippet* snippet = new LightSnippet(light);
		lightSnippets.push_back(snippet);
	}
}