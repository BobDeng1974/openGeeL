#include "../../renderer/transformation/transform.h"
#include "../../renderer/cameras/camera.h"
#include "../../renderer/cameras/perspectivecamera.h"
#include "../../renderer/materials/material.h"
#include "../../renderer/materials/defaultmaterial.h"
#include "../../renderer/materials/genericmaterial.h"
#include "../../renderer/meshes/meshrenderer.h"
#include "../../renderer/lights/light.h"
#include  "../../renderer/sceneobject.h"
#include "../../renderer/shadowmapping/simpleshadowmap.h"
#include "../snippets/guisnippets.h"
#include "../guiwrapper.h"
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


	MeshRendererSnippet::MeshRendererSnippet(MeshRenderer& mesh) : SceneObjectSnippet(mesh), mesh(mesh) {}

	void MeshRendererSnippet::draw(GUIContext* context) {

		std::string number = "#" + std::to_string(mesh.transform.getID());
		std::string id = mesh.getName() + " " + number;
		if (nk_tree_push(context, NK_TREE_NODE, id.c_str(), NK_MINIMIZED)) {
			SceneObjectSnippet::draw(context);

			std::string materials = "Materials " + number;
			if (nk_tree_push(context, NK_TREE_NODE, materials.c_str(), NK_MINIMIZED)) {
				
				unsigned int counter = 0;
				mesh.iterateMaterials([&](MaterialContainer& container) {
					std::string name = "Material " + std::to_string(counter) + ": " + container.name;
					nk_layout_row_dynamic(context, 30, 1);
					nk_label(context, name.c_str(), NK_TEXT_LEFT);

					//TODO: Make this less horrible and remove casts
					DefaultMaterialContainer* def = static_cast<DefaultMaterialContainer*>(&container);
					if (def != nullptr)
						GUISnippets::drawMaterial(context, def);
					else {
						GenericMaterialContainer* gen = static_cast<GenericMaterialContainer*>(&container);
						if (gen != nullptr)
							GUISnippets::drawMaterial(context, gen);
					}

					counter++;
				});

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

		std::string id = light.getName() + " #" + std::to_string(light.transform.getID());
		if (nk_tree_push(context, NK_TREE_NODE, id.c_str(), NK_MINIMIZED)) {
			SceneObjectSnippet::draw(context);

			vec3 color = light.getColor();
			GUISnippets::drawColor(context, color);
			light.setColor(color);
			
			if (snippet != nullptr)
				snippet->draw(context);
			
			nk_tree_pop(context);
		}
	}

	std::string LightSnippet::toString() const {
		return "Light";
	}


	CameraSnippet::CameraSnippet(SceneCamera& cam) : SceneObjectSnippet(cam), cam(cam) {}

	void CameraSnippet::draw(GUIContext* context) {
		SceneObjectSnippet::draw(context);

		float speed = GUISnippets::drawBarFloat(context, cam.getSpeed(), 0.f, 30.f, 0.1f, "Speed");
		cam.setSpeed(speed);

		float sensitivity = GUISnippets::drawBarFloat(context, cam.getSensitivity(), 0.f, 30.f, 0.1f, "Sensitivity");
		cam.setSensitivity(sensitivity);
	}

	std::string CameraSnippet::toString() const {
		return "Camera";
	}


	PerspectiveCameraSnippet::PerspectiveCameraSnippet(PerspectiveCamera& pcam) 
		: CameraSnippet(pcam), pcam(pcam) {}

	void PerspectiveCameraSnippet::draw(GUIContext* context) {

		std::string id = pcam.getName() + " #" + std::to_string(pcam.transform.getID());
		if (nk_tree_push(context, NK_TREE_NODE, id.c_str(), NK_MINIMIZED)) {
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

				float scale = GUISnippets::drawBarFloat(context, map.getSoftShadowScale(), 0.f, 20.f, 0.1f, "Scale");
				map.setSoftShadowScale(scale);
			}
		}
	}

	std::string ShadowMapSnippet::toString() const {
		return "Shadow Map";
	}
}