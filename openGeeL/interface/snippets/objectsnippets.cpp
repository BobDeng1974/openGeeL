#include "transformation/transform.h"
#include "cameras/camera.h"
#include "cameras/perspectivecamera.h"
#include "materials/material.h"
#include "materials/defaultmaterial.h"
#include "materials/genericmaterial.h"
#include "meshes/mesh.h"
#include "meshes/meshrenderer.h"
#include "lights/light.h"
#include "sceneobject.h"
#include "shadowmapping/simpleshadowmap.h"
#include "snippets/guisnippets.h"
#include "guiwrapper.h"
#include "objectsnippets.h"

namespace geeL {


	SceneObjectSnippet::SceneObjectSnippet(SceneObject& obj) : obj(obj) {}

	void SceneObjectSnippet::draw(GUIContext* context) {

		int active = !obj.isActive();
		nk_layout_row_dynamic(context, 30, 2);
		nk_checkbox_label(context, "Active", &active);
		obj.setActive(!active);

		nk_label(context, obj.transform.isStatic ? "STATIC" : "DYNAMIC", NK_TEXT_CENTERED);

		GUISnippets::drawTransform(context, obj.transform);
	}

	std::string SceneObjectSnippet::toString() const {
		return "Scene Object";
	}


	MeshRendererSnippet::MeshRendererSnippet(MeshRenderer& mesh) 
		: SceneObjectSnippet(mesh), mesh(mesh) {}

	void MeshRendererSnippet::draw(GUIContext* context) {
		unsigned int id = mesh.transform.getID();

		std::string number = "#" + std::to_string(mesh.transform.getID());
		std::string name = mesh.getName() + " " + number;
		if (nk_tree_push_id(context, NK_TREE_NODE, name.c_str(), NK_MINIMIZED, id)) {
			SceneObjectSnippet::draw(context);

			unsigned int materialsID = id + 1;

			if (nk_tree_push_id(context, NK_TREE_NODE, "Materials", NK_MINIMIZED, materialsID)) {
				auto defaultColor = context->style.tab.node_minimize_button.text_normal;

				unsigned int counter = 0;
				mesh.iterate([&](const MeshInstance& mesh, const Material& material) {
					MaterialContainer& container = material.getMaterialContainer();
					unsigned int containerID = id + counter;

					glm::vec3 c(container.getVectorValue("Color") * 256.f);
					context->style.tab.node_minimize_button.text_normal = nk_rgb(c.r, c.g, c.b);


					std::string name = container.name + " (" + mesh.getName() + ")";
					if (nk_tree_push_id(context, NK_TREE_NODE, name.c_str(), NK_MINIMIZED, containerID)) {

						//TODO: Make this less horrible and remove casts
						DefaultMaterialContainer* def = dynamic_cast<DefaultMaterialContainer*>(&container);
						if (def != nullptr)
							GUISnippets::drawMaterial(context, def);
						else {
							GenericMaterialContainer* gen = dynamic_cast<GenericMaterialContainer*>(&container);
							if (gen != nullptr)
								GUISnippets::drawMaterial(context, gen);
						}

						nk_tree_pop(context);
					}

					counter++;
				});


				context->style.tab.node_minimize_button.text_normal = defaultColor;

				nk_tree_pop(context);
			}

			nk_tree_pop(context);
		}
	}

	std::string MeshRendererSnippet::toString() const {
		return "Mesh Renderer";
	}


	LightSnippet::LightSnippet(Light& light) 
		: SceneObjectSnippet(light), light(light), snippet(nullptr) {}

	LightSnippet::LightSnippet(Light& light, ShadowMapSnippet& snippet) 
		: SceneObjectSnippet(light), light(light), snippet(&snippet) {}


	void LightSnippet::draw(GUIContext* context) {
		unsigned int id = light.transform.getID();

		auto defaultColor = context->style.tab.node_minimize_button.text_normal;
		glm::vec3 c(light.getColor() * 256.f);
		context->style.tab.node_minimize_button.text_normal = nk_rgb(c.r, c.g, c.b);

		std::string name = light.getName() + " #" + std::to_string(light.transform.getID());
		if (nk_tree_push_id(context, NK_TREE_NODE, name.c_str(), NK_MINIMIZED, id)) {
			SceneObjectSnippet::draw(context);

			vec3 color = light.getColor();
			GUISnippets::drawColor(context, color);
			light.setColor(color);

			float intensity = GUISnippets::drawBarFloatLogarithmic(context, light.getIntensity(), 0.01f, 2000.f, 0.01f, "Intensity");
			light.setIntensity(intensity);
			
			if (snippet != nullptr)
				snippet->draw(context);
			
			nk_tree_pop(context);
		}

		context->style.tab.node_minimize_button.text_normal = defaultColor;
	}

	std::string LightSnippet::toString() const {
		return "Light";
	}


	CameraSnippet::CameraSnippet(SceneCamera& cam) : SceneObjectSnippet(cam), cam(cam) {}

	void CameraSnippet::draw(GUIContext* context) {
		SceneObjectSnippet::draw(context);
	}

	std::string CameraSnippet::toString() const {
		return "Camera";
	}


	PerspectiveCameraSnippet::PerspectiveCameraSnippet(PerspectiveCamera& pcam) 
		: CameraSnippet(pcam), pcam(pcam) {}

	void PerspectiveCameraSnippet::draw(GUIContext* context) {
		unsigned int id = pcam.transform.getID();

		std::string name = pcam.getName() + " #" + std::to_string(pcam.transform.getID());
		if (nk_tree_push_id(context, NK_TREE_NODE, name.c_str(), NK_MINIMIZED, id)) {
			CameraSnippet::draw(context);

			float fov = GUISnippets::drawBarFloat(context, pcam.getFieldOfView(), 1.f, 170.f, 1.f, "FoV");
			pcam.setFieldOfView(fov);

			float near = GUISnippets::drawBarFloat(context, pcam.getNearPlane(), 0.1f, 100.f, 0.1f, "Near");
			pcam.setNearPlane(near);

			float far = GUISnippets::drawBarFloat(context, pcam.getFarPlane(), 10.f, 1000.f, 1.f, "Far");
			pcam.setFarPlane(far);

			nk_tree_pop(context);
		}
	}

	std::string PerspectiveCameraSnippet::toString() const {
		return "Perspective Camera";
	}


	ShadowMapSnippet::ShadowMapSnippet(SimpleShadowMap& map) : map(map) {}

	void ShadowMapSnippet::draw(GUIContext* context) {
		ShadowMapType type = map.getType();
		
		if (type != ShadowMapType::None) {
			nk_label(context, "Shadow Map", NK_TEXT_LEFT);

			float intensity = GUISnippets::drawBarFloat(context, map.getIntensity(), 0.f, 1.f, 0.001f, "Shadow Intensity");
			map.setIntensity(intensity);

			float bias = GUISnippets::drawBarFloat(context, map.getShadowBias(), 0.f, 0.02f, 0.0001f, "Shadow Bias");
			map.setShadowBias(bias);


			if (type == ShadowMapType::Soft) {
				int resolution = GUISnippets::drawBarInteger(context, map.getSoftShadowResolution(), 1, 100, 1, "Resolution");
				map.setSoftShadowResolution(resolution);

				float scale = GUISnippets::drawBarFloatLogarithmic(context, map.getSoftShadowScale(), 0.f, 15.f, 0.1f, "Scale");
				map.setSoftShadowScale(scale);
			}
		}
	}

	std::string ShadowMapSnippet::toString() const {
		return "Shadow Map";
	}
}