#include "../cameras/camera.h"
#include "ssrr.h"

using namespace std;

namespace geeL {

	SSRR::SSRR(const Camera& camera) 
		: WorldPostProcessingEffect("renderer/postprocessing/ssrr.frag"), camera(camera) {}


	void SSRR::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("gPositionDepth", shader.mapOffset + 1);
		shader.setInteger("gNormalMet", shader.mapOffset + 2);

		shader.setMat4("projection", camera.getProjectionMatrix());
	}

	WorldMaps SSRR::requiredWorldMaps() const {
		return (WorldMaps::RenderedImage | WorldMaps::PositionDepth | WorldMaps::NormalMetallic);
	}

	WorldMatrices SSRR::requiredWorldMatrices() const {
		return WorldMatrices::None;
	}

	WorldVectors SSRR::requiredWorldVectors() const {
		return WorldVectors::None;
	}

	list<WorldMaps> SSRR::requiredWorldMapsList() const {
		return { WorldMaps::RenderedImage, WorldMaps::PositionDepth, WorldMaps::NormalMetallic };
	}

	void SSRR::addWorldInformation(list<unsigned int> maps,
		list<glm::mat4> matrices, list<glm::vec3> vectors) {

		if (maps.size() != 3)
			throw "Wrong number of texture maps attached to SSAO";

		setBuffer(maps);
	}
}
