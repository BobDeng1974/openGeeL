#include "transformation/transform.h"
#include "cameras/camera.h"
#include "cameras/perspectivecamera.h"
#include "materials/material.h"
#include "materials/defaultmaterial.h"
#include "materials/genericmaterial.h"
#include "meshes/mesh.h"
#include "meshes/meshrenderer.h"
#include "lights/light.h"
#include "lights/pointlight.h"
#include "lights/spotlight.h"
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



	SingleMeshRendererSnippet::SingleMeshRendererSnippet(MeshRenderer & mesh)
		: SceneObjectSnippet(mesh), mesh(mesh) {}

	void SingleMeshRendererSnippet::draw(GUIContext* context) {
		unsigned int id = mesh.getID();

		MaterialContainer& container = mesh.getMaterial().getMaterialContainer();
		auto defaultColor = context->style.tab.node_minimize_button.text_normal;
		glm::vec3 c(container.getVectorValue("Color") * 256.f);
		context->style.tab.node_minimize_button.text_normal = nk_rgb(c.r, c.g, c.b);

		std::string number = "#" + std::to_string(id);
		std::string name = mesh.getName() + " " + number;
		if (nk_tree_push_id(context, NK_TREE_NODE, name.c_str(), NK_MINIMIZED, id)) {
			SceneObjectSnippet::draw(context);

			unsigned int materialsID = id + 1;
			//std::string name = container.name + " (" + mesh.getName() + ")";

			if (nk_tree_push_id(context, NK_TREE_NODE, "Material", NK_MINIMIZED, materialsID)) {
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

			nk_tree_pop(context);
		}

		context->style.tab.node_minimize_button.text_normal = defaultColor;
	}

	std::string SingleMeshRendererSnippet::toString() const {
		return "Single Mesh Renderer";
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

			PointLight* pl = dynamic_cast<PointLight*>(&light);
			if (pl) {
				float volStrength = GUISnippets::drawBarFloat(context, pl->getVolumetricStrength(), 0.f, 0.05f, 0.001f, "Vol Strength");
				pl->setVolumetricStrength(volStrength);

				float volDensity = GUISnippets::drawBarFloat(context, pl->getVolumetricDensity(), 0.01f, 10.f, 0.01f, "Vol Density");
				pl->setVolumetricDensity(volDensity);
			}
			else {
				SpotLight* sl = dynamic_cast<SpotLight*>(&light);
				if (sl) {
					float angle = GUISnippets::drawBarFloat(context, sl->getAngleDegree(), 0.f, 180.f, 1.f, "Angle");
					sl->setAngleDegree(angle);

					float border = GUISnippets::drawBarFloat(context, sl->getOuterAngleDegree(), 0.f, 15.f, 0.1f, "Border");
					sl->setOuterAngleDegree(border);
				}
			}
			
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


	ShadowMapSnippet::ShadowMapSnippet(SimpleShadowmap& map) : map(map) {}

	void ShadowMapSnippet::draw(GUIContext* context) {
		ShadowMapType type = map.getType();
		
		if (type != ShadowMapType::None) {

			nk_layout_row_dynamic(context, 30, 3);
			nk_label(context, "Shadow Map", NK_TEXT_LEFT);
			
			unsigned int resolution = map.getScreenResolution().getWidth();
			nk_label(context, "Dynamic Size: ", NK_TEXT_RIGHT);
			nk_label(context, std::to_string(resolution).c_str(), NK_TEXT_CENTERED);

			if(!map.getLight().isStatic())
				drawResolutionMeter(context);

			float intensity = GUISnippets::drawBarFloat(context, map.getIntensity(), 0.f, 1.f, 0.001f, "Shadow Intensity");
			map.setIntensity(intensity);

			float bias = GUISnippets::drawBarFloat(context, map.getShadowBias(), 0.f, 0.02f, 0.0001f, "Shadow Bias");
			map.setShadowBias(bias);

			float plane = GUISnippets::drawBarFloatLogarithmic(context, map.getFarPlane(), 1.f, 500.f, 0.0001f, "Far Plane");
			map.setFarPlane(plane);

			if (type == ShadowMapType::Soft) {
				int resolution = GUISnippets::drawBarInteger(context, map.getSoftShadowResolution(), 1, 10, 1, "Sample Size");
				map.setSoftShadowResolution(resolution);

				float scale = GUISnippets::drawBarFloatLogarithmic(context, map.getSoftShadowScale(), 0.f, 15.f, 0.1f, "Sample Scale");
				map.setSoftShadowScale(scale);
			}
		}
	}

	std::string ShadowMapSnippet::toString() const {
		return "Shadow Map";
	}

	void ShadowMapSnippet::drawResolutionMeter(GUIContext* context) {

		int resNumber = 0;
		int res = static_cast<int>(map.getShadowResolution());
		switch (map.getShadowResolution()) {
			case ShadowmapResolution::Small:
				resNumber = 1;
				break;
			case ShadowmapResolution::Medium:
				resNumber = 2;
				break;
			case ShadowmapResolution::High:
				resNumber = 3;
				break;
			case ShadowmapResolution::VeryHigh:
				resNumber = 4;
				break;
			case ShadowmapResolution::Large:
				resNumber = 5;
				break;
			case ShadowmapResolution::Huge:
				resNumber = 6;
				break;
		}

		nk_layout_row_dynamic(context, 30, 3);
		nk_label(context, "Size", NK_TEXT_CENTERED);
		nk_slider_int(context, 0, &resNumber, 6, 1);

		ShadowmapResolution newRes = ShadowmapResolution::Tiny;
		switch (resNumber) {
			case 1:
				newRes = ShadowmapResolution::Small;
				break;
			case 2:
				newRes = ShadowmapResolution::Medium;
				break;
			case 3:
				newRes = ShadowmapResolution::High;
				break;
			case 4:
				newRes = ShadowmapResolution::VeryHigh;
				break;
			case 5:
				newRes = ShadowmapResolution::Large;
				break;
			case 6:
				newRes = ShadowmapResolution::Huge;
				break;
		}

		std::string valName = std::to_string(static_cast<unsigned int>(newRes));
		nk_label(context, valName.c_str(), NK_TEXT_CENTERED);

		map.setShadowResolution(newRes);
	}


	

}