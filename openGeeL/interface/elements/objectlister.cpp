#include "../../renderer/transformation/transform.h"
#include "../../renderer/meshes/meshrenderer.h"
#include "../../renderer/lighting/light.h"
#include "../../renderer/lighting/directionallight.h"
#include "../../renderer/lighting/spotlight.h"
#include "../../renderer/lighting/pointlight.h"
#include "../../renderer/lighting/lightmanager.h"
#include "../../renderer/scene.h"
#include "../guiwrapper.h"
#include "../guisnippets.h"
#include "objectlister.h"
#include <iostream>

namespace geeL {

	ObjectLister::ObjectLister(RenderScene& scene) : scene(scene) {}


	void ObjectLister::draw(GUIContext* context) {

		if (nk_begin(context, "Scene", nk_rect(20, 20, 350, 350),
		NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
		NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

			if (nk_tree_push(context, NK_TREE_NODE, "Lights", NK_MAXIMIZED)) {

				LightManager& manager = scene.lightManager;

				for (auto it = manager.directionalLightsBegin(); it != manager.directionalLightsEnd(); it++) {
					Light& light = **it;
					GUISnippets::drawLight(context, light);
				}

				for (auto it = manager.spotLightsBegin(); it != manager.spotLightsEnd(); it++) {
					Light& light = **it;
					GUISnippets::drawLight(context, light);
				}

				for (auto it = manager.pointLightsBegin(); it != manager.pointLightsEnd(); it++) {
					Light& light = **it;
					GUISnippets::drawLight(context, light);
				}
				

				nk_tree_pop(context);
			}

			if (nk_tree_push(context, NK_TREE_NODE, "Objects", NK_MINIMIZED)) {

				for (auto it = scene.renderObjectsBegin(); it != scene.renderObjectsEnd(); it++) {
					MeshRenderer* renderer = *it;
					Transform& transform = renderer->transform;

					GUISnippets::drawTransform(context, transform);
				}

				nk_tree_pop(context);
			}

		}

		nk_end(context);
		
	}


}