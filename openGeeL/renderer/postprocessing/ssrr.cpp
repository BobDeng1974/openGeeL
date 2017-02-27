#include "../cameras/camera.h"
#include "ssrr.h"

using namespace std;

namespace geeL {

	SSRR::SSRR(const Camera& camera) 
		: PostProcessingEffect("renderer/postprocessing/ssrr.frag"), camera(camera) {}


	void SSRR::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("gPositionDepth", shader.mapOffset + 1);
		shader.setInteger("gNormalMet", shader.mapOffset + 2);
		shader.setInteger("gSpecular", shader.mapOffset + 3);

		shader.setMat4("projection", camera.getProjectionMatrix());
		shader.setInteger("effectOnly", onlyEffect);
	}

	void SSRR::addWorldInformation(map<WorldMaps, unsigned int> maps, map<WorldMatrices,
		const glm::mat4*> matrices,
		map<WorldVectors, const glm::vec3*> vectors) {

		addBuffer({ maps[WorldMaps::PositionDepth], maps[WorldMaps::NormalMetallic] });
	}


}
