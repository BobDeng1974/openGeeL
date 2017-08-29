#include "cameras/camera.h"
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "lights/lightmanager.h"
#include "renderscene.h"
#include "ibl.h"

namespace geeL {

	ImageBasedLighting::ImageBasedLighting(RenderScene& scene)
		: SceneRender(scene), PostProcessingEffectFS("renderer/lighting/deferredlighting.vert",
			"renderer/lighting/ibl.frag") {}


	void ImageBasedLighting::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		scene.getLightmanager().addReflectionProbes(shader);
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void ImageBasedLighting::draw() {
		PostProcessingEffectFS::draw();
	}

	void ImageBasedLighting::bindValues() {
		scene.getLightmanager().bindReflectionProbes(*camera, shader, ShaderTransformSpace::View);
		shader.bind<glm::mat4>(invViewLocation, camera->getInverseViewMatrix());
		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
	}

	void ImageBasedLighting::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		addImageBuffer(*maps[WorldMaps::Diffuse], "gDiffuse");
		addImageBuffer(*maps[WorldMaps::PositionRoughness], "gPositionRoughness");
		addImageBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");

		auto ssao = maps.find(WorldMaps::SSAO);
		if (ssao != maps.end()) {
			const Texture& texture = *ssao->second;

			addImageBuffer(texture, "ssao");
			shader.bind<int>("useSSAO", 1);
		}
	}
}