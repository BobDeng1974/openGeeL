#include "../../renderer/transformation/transform.h"
#include "../../renderer/materials/material.h"
#include "../../renderer/materials/defaultmaterial.h"
#include "../../renderer/materials/genericmaterial.h"
#include "../../renderer/meshes/meshrenderer.h"
#include "../../renderer/lighting/light.h"
#include  "../../renderer/sceneobject.h"
#include "../snippets/guisnippets.h"
#include "../guiwrapper.h"
#include "objectsnippets.h"

namespace geeL {


	SceneObjectSnippet::SceneObjectSnippet(SceneObject& obj) : obj(obj) {}

	void SceneObjectSnippet::draw(GUIContext* context) {

		int active = !obj.isActive();
		nk_checkbox_label(context, "Active", &active);
		obj.setActive(!active);

		GUISnippets::drawTransform(context, obj.transform);
	}

	std::string SceneObjectSnippet::toString() const {
		return "Scene Object";
	}


	MeshRendererSnippet::MeshRendererSnippet(MeshRenderer& mesh) : SceneObjectSnippet(mesh), mesh(mesh) {}

	void MeshRendererSnippet::draw(GUIContext* context) {

		std::string number = "#" + std::to_string(mesh.transform.getID());
		std::string id = mesh.getName() + " " + number;
		if (nk_tree_push(context, NK_TREE_NODE, id.c_str(), NK_MINIMIZED)) {
			SceneObjectSnippet::draw(context);

			std::string materials = "Materials " + number;
			if (nk_tree_push(context, NK_TREE_NODE, materials.c_str(), NK_MINIMIZED)) {
				
				unsigned int counter = 0;
				for (auto it = mesh.deferredMaterialsBegin(); it != mesh.deferredMaterialsEnd(); it++) {
					MaterialContainer* cont = &(*it).second->container;

					std::string name = "Material " + std::to_string(counter);
					nk_layout_row_dynamic(context, 30, 1);
					nk_label(context, name.c_str(), NK_TEXT_LEFT);

					//TODO: Make this less horrible and remove casts
					DefaultMaterialContainer* def = static_cast<DefaultMaterialContainer*>(cont);
					if(def != nullptr)
						GUISnippets::drawMaterial(context, def);
					else {
						GenericMaterialContainer* gen = static_cast<GenericMaterialContainer*>(cont);
						if(gen != nullptr)
							GUISnippets::drawMaterial(context, gen);
					}

					counter++;
				}
				
				nk_tree_pop(context);
			}

			nk_tree_pop(context);
		}
	}

	std::string MeshRendererSnippet::toString() const {
		return "Mesh Renderer";
	}


	LightSnippet::LightSnippet(Light& light) : SceneObjectSnippet(light), light(light) {}

	void LightSnippet::draw(GUIContext* context) {

		std::string id = light.getName() + " #" + std::to_string(light.transform.getID());
		if (nk_tree_push(context, NK_TREE_NODE, id.c_str(), NK_MINIMIZED)) {
			SceneObjectSnippet::draw(context);
			GUISnippets::drawColor(context, light.diffuse);

			float bias = GUISnippets::drawBarFloat(context, light.getShadowBias(), 0.f, 0.02f, 0.0001f, "Shadow Bias");
			light.setShadowBias(bias);
			
			nk_tree_pop(context);
		}
	}

	std::string LightSnippet::toString() const {
		return "Light";
	}
}