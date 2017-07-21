#include "../cameras/camera.h"
#include "../shader/rendershader.h"
#include "../shader/sceneshader.h"
#include "../lights/lightmanager.h"
#include "../scene.h"
#include "deferredlighting.h"

namespace geeL {

	DeferredLighting::DeferredLighting(RenderScene& scene) 
		: SceneRender(scene, "renderer/lighting/deferredlighting.vert", 
			"renderer/lighting/deferredlighting.frag") {}


	void DeferredLighting::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		scene.lightManager.bindShadowMaps(shader);

		projectionLocation = shader.getLocation("projection");
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void DeferredLighting::bindValues() {
		scene.lightManager.bind(*camera, shader, ShaderTransformSpace::View);

		shader.bind<glm::mat4>(projectionLocation, camera->getProjectionMatrix());
		shader.bind<glm::mat4>(invViewLocation, camera->getInverseViewMatrix());
		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
	}


	void DeferredLighting::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		addImageBuffer(*maps[WorldMaps::Diffuse], "gDiffuse");
		addImageBuffer(*maps[WorldMaps::PositionRoughness], "gPositionRoughness");
		addImageBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");

		auto emissivity = maps.find(WorldMaps::Emissivity);
		if (emissivity != maps.end()) {
			const Texture& texture = *emissivity->second;

			addImageBuffer(texture, "gEmissivity");
			shader.use();
			shader.bind<int>("useEmissivity", 1);
		}
	}
}