#include "../../renderer/transformation/transform.h"
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

		std::string id = "Mesh #" + std::to_string(mesh.transform.getID());
		if (nk_tree_push(context, NK_TREE_NODE, id.c_str(), NK_MINIMIZED)) {
			SceneObjectSnippet::draw(context);

			nk_tree_pop(context);
		}
	}

	std::string MeshRendererSnippet::toString() const {
		return "Mesh Renderer";
	}


	LightSnippet::LightSnippet(Light& light) : SceneObjectSnippet(light), light(light) {}

	void LightSnippet::draw(GUIContext* context) {

		std::string id = "Light #" + std::to_string(light.transform.getID());
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