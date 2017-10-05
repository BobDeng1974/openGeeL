#include "cameras/camera.h"
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "lights/lightmanager.h"
#include "renderscene.h"
#include "deferredlighting.h"

namespace geeL {

	DeferredLighting::DeferredLighting(RenderScene& scene) 
		: SceneRender(scene), PostProcessingEffectFS( "renderer/lighting/deferredlighting.vert",
			"renderer/lighting/deferredlighting.frag") {}


	void DeferredLighting::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		scene.init();

		LightManager& manager = scene.getLightmanager();
		manager.addShaderListener(shader);

		projectionLocation = shader.getLocation("projection");
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void DeferredLighting::draw() {
		PostProcessingEffectFS::draw();
	}

	void DeferredLighting::bindValues() {
		scene.getLightmanager().bind(shader, ShaderTransformSpace::View, camera);

		shader.bind<glm::mat4>(projectionLocation, camera->getProjectionMatrix());
		shader.bind<glm::mat4>(invViewLocation, camera->getInverseViewMatrix());
		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
	}


	void DeferredLighting::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		addTextureSampler(*maps[WorldMaps::Diffuse], "gDiffuse");
		addTextureSampler(*maps[WorldMaps::PositionRoughness], "gPositionRoughness");
		addTextureSampler(*maps[WorldMaps::NormalMetallic], "gNormalMet");

		auto emissivity = maps.find(WorldMaps::Emissivity);
		if (emissivity != maps.end()) {
			const Texture& texture = *emissivity->second;

			addTextureSampler(texture, "gEmissivity");
			shader.bind<int>("useEmissivity", 1);
		}
	}




	TiledDeferredLighting::TiledDeferredLighting(RenderScene& scene)
		: SceneRender(scene), PostProcessingEffectCS("renderer/lighting/tileddeferred.com.glsl", Resolution(16, 16)) {}


	void TiledDeferredLighting::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectCS::init(screen, buffer, resolution);

		scene.init();

		LightManager& manager = scene.getLightmanager();
		manager.addShaderListener(shader);

		projectionLocation = shader.getLocation("projection");
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void TiledDeferredLighting::draw() {
		PostProcessingEffectCS::draw();
	}

	void TiledDeferredLighting::bindValues() {
		scene.getLightmanager().bind(shader, ShaderTransformSpace::View, camera);

		shader.bind<glm::mat4>(projectionLocation, camera->getProjectionMatrix());
		shader.bind<glm::mat4>(invViewLocation, camera->getInverseViewMatrix());
		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
	}


	void TiledDeferredLighting::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		addTextureSampler(*maps[WorldMaps::Diffuse], "gDiffuse");
		addTextureSampler(*maps[WorldMaps::PositionRoughness], "gPositionRoughness");
		addTextureSampler(*maps[WorldMaps::NormalMetallic], "gNormalMet");

		auto emissivity = maps.find(WorldMaps::Emissivity);
		if (emissivity != maps.end()) {
			const Texture& texture = *emissivity->second;

			addTextureSampler(texture, "gEmissivity");
			shader.bind<int>("useEmissivity", 1);
		}
	}


}