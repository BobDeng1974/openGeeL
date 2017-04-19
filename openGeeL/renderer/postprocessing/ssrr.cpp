#include "../cameras/camera.h"
#include "../framebuffer/framebuffer.h"
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
		shader.setMat4(projectionLocation, camera->getProjectionMatrix());
	}

	void SSRR::addWorldInformation(map<WorldMaps, unsigned int> maps) {
		addBuffer({ maps[WorldMaps::DiffuseRoughness], 
			maps[WorldMaps::PositionDepth], maps[WorldMaps::NormalMetallic] });
	}

}
