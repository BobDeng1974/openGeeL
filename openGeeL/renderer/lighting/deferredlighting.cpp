#include "../cameras/camera.h"
#include "../shader/shader.h"
#include "../shader/sceneshader.h"
#include "../lights/lightmanager.h"
#include "../scene.h"
#include "deferredlighting.h"

namespace geeL {

	DeferredLighting::DeferredLighting(RenderScene& scene) 
		: SceneRender(scene, "renderer/shaders/deferredlighting.vert", 
			"renderer/shaders/cooktorrancedeferred.frag") {}


	void DeferredLighting::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		scene.lightManager.bindReflectionProbes(shader);
		scene.lightManager.bindShadowMaps(shader);

		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void DeferredLighting::bindValues() {
		scene.lightManager.bindReflectionProbes(*camera, shader, ShaderTransformSpace::View);
		scene.lightManager.bind(*camera, shader, ShaderTransformSpace::View);
		shader.setMat4(invViewLocation, camera->getInverseViewMatrix());
		shader.setVector3(originLocation, camera->GetOriginInViewSpace());
	}


	void DeferredLighting::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		addBuffer(*maps[WorldMaps::DiffuseRoughness], "gDiffuseSpec");
		addBuffer(*maps[WorldMaps::PositionDepth], "gPositionDepth");
		addBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");
		
		auto ssao = maps.find(WorldMaps::SSAO);
		if (ssao != maps.end()) {
			const Texture& texture = *ssao->second;

			addBuffer(texture, "ssao");
			shader.use();
			shader.setInteger("useSSAO", 1);
		}
	}
}