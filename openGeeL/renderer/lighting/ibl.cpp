#include "../cameras/camera.h"
#include "../shader/rendershader.h"
#include "../shader/sceneshader.h"
#include "../lights/lightmanager.h"
#include "../scene.h"
#include "ibl.h"

namespace geeL {

	ImageBasedLighting::ImageBasedLighting(RenderScene& scene)
		: SceneRender(scene, "renderer/lighting/deferredlighting.vert",
			"renderer/lighting/ibl.frag") {}


	void ImageBasedLighting::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		scene.lightManager.addReflectionProbes(shader);
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void ImageBasedLighting::bindValues() {
		scene.lightManager.bindReflectionProbes(*camera, shader, ShaderTransformSpace::View);
		shader.setMat4(invViewLocation, camera->getInverseViewMatrix());
		shader.setVector3(originLocation, camera->GetOriginInViewSpace());
	}

	void ImageBasedLighting::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		addImageBuffer(*maps[WorldMaps::Diffuse], "gDiffuse");
		addImageBuffer(*maps[WorldMaps::PositionRoughness], "gPositionRoughness");
		addImageBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");

		auto ssao = maps.find(WorldMaps::SSAO);
		if (ssao != maps.end()) {
			const Texture& texture = *ssao->second;

			addImageBuffer(texture, "ssao");
			shader.use();
			shader.setInteger("useSSAO", 1);
		}
	}
}