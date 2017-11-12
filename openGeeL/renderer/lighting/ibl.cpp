#include "cameras/camera.h"
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "lights/lightmanager.h"
#include "renderscene.h"
#include "ibl.h"

namespace geeL {

	ImageBasedLighting::ImageBasedLighting(RenderScene& scene)
		: SceneRender(scene)
		, AdditiveEffect("renderer/lighting/deferredlighting.vert",
			"renderer/lighting/ibl.frag") {}


	void ImageBasedLighting::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		scene.getLightmanager().addReflectionProbes(shader);
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void ImageBasedLighting::draw() {
		PostProcessingEffectFS::draw();
	}

	void ImageBasedLighting::bindValues() {
		scene.getLightmanager().bindReflectionProbes(*camera, shader, ShaderTransformSpace::View);
		camera->bindInverseViewMatrix(shader, invViewLocation);

		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
	}

	void ImageBasedLighting::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		addTextureSampler(*maps[WorldMaps::Diffuse], "gDiffuse");
		addTextureSampler(*maps[WorldMaps::PositionRoughness], "gPositionRoughness");
		addTextureSampler(*maps[WorldMaps::NormalMetallic], "gNormalMet");

		auto ssao = maps.find(WorldMaps::Occlusion);
		if (ssao != maps.end()) {
			const Texture& texture = *ssao->second;

			addTextureSampler(texture, "ssao");
			shader.bind<int>("useSSAO", 1);
		}
	}
}