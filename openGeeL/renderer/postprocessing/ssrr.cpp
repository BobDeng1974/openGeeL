#include "../cameras/camera.h"
#include "../utility/framebuffer.h"
#include "ssrr.h"
#include <iostream>

using namespace std;

namespace geeL {

	SSRR::SSRR() : PostProcessingEffect("renderer/postprocessing/ssrr.frag"){}


	void SSRR::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("gSpecular", shader.mapOffset + 1);
		shader.setInteger("gPositionDepth", shader.mapOffset + 2);
		shader.setInteger("gNormalMet", shader.mapOffset + 3);

		shader.setInteger("effectOnly", onlyEffect);

		projectionLocation = shader.getLocation("projection");
	}

	void SSRR::bindValues() {
		shader.setMat4(projectionLocation, *projectionMatrix);
	}

	void SSRR::addWorldInformation(map<WorldMaps, unsigned int> maps, map<WorldMatrices,
		const glm::mat4*> matrices,
		map<WorldVectors, const glm::vec3*> vectors) {

		addBuffer({ maps[WorldMaps::DiffuseRoughness], maps[WorldMaps::PositionDepth], maps[WorldMaps::NormalMetallic] });
		projectionMatrix = matrices[WorldMatrices::Projection];
	}

}
