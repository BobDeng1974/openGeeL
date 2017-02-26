#include "../transformation/transform.h"
#include "../lighting/spotlight.h"
#include "volumetriclight.h"

#include "../scene.h"
#include "../cameras/camera.h"


using namespace glm;
using namespace std;

namespace geeL {

	VolumetricLight::VolumetricLight(const RenderScene& scene, const SpotLight& light, float density, float minDistance, int samples)
		: WorldPostProcessingEffect("renderer/postprocessing/volumetriclight.frag"), 
			scene(scene), light(light), density(density), minDistance(minDistance), samples(samples) {}


	void VolumetricLight::init(ScreenQuad& screen) {
		PostProcessingEffect::init(screen);

		buffers.push_back(light.getShadowMapID());
	}

	void VolumetricLight::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("gPositionDepth", shader.mapOffset + 1);
		shader.setInteger("shadowMap", shader.mapOffset + 2);

		shader.setInteger("effectOnly", onlyEffect);
		shader.setMat4("inverseView", *inverseView);
		shader.setFloat("minCutoff", minDistance);
		shader.setFloat("density", density);
		shader.setInteger("samples", samples);

		light.deferredBind(scene, shader, "light.");
	}


	WorldMaps VolumetricLight::requiredWorldMaps() const {
		return (WorldMaps::RenderedImage | WorldMaps::PositionDepth);
	}

	WorldMatrices VolumetricLight::requiredWorldMatrices() const {
		return WorldMatrices::InverseView;
	}

	WorldVectors VolumetricLight::requiredWorldVectors() const {
		return WorldVectors::None;
	}

	std::list<WorldMaps> VolumetricLight::requiredWorldMapsList() const {
		return{ WorldMaps::RenderedImage, WorldMaps::PositionDepth };
	}

	std::list<WorldMatrices> VolumetricLight::requiredWorldMatricesList() const {
		return{ WorldMatrices::InverseView };
	}


	void VolumetricLight::addWorldInformation(list<unsigned int> maps,
		list<const glm::mat4*> matrices, list<const vec3*> vectors) {

		if (maps.size() != 2)
			throw "Wrong number of texture maps attached to volumetric light";

		setBuffer(maps);

		if (matrices.size() != 1)
			throw "Wrong numbers of matrices attached to volumetric light";

		inverseView = matrices.front();
	}
}