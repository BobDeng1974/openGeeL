#include "../cameras/camera.h"
#include "../framebuffer/framebuffer.h"
#include "ssrr.h"
#include <iostream>

using namespace std;

namespace geeL {

	SSRR::SSRR() : PostProcessingEffect("renderer/postprocessing/ssrr.frag"){}


	void SSRR::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		shader.setInteger("effectOnly", onlyEffect);
		projectionLocation = shader.getLocation("projection");
	}

	void SSRR::bindValues() {
		shader.setMat4(projectionLocation, camera->getProjectionMatrix());
	}

	void SSRR::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addBuffer(*maps[WorldMaps::DiffuseRoughness], "gSpecular");
		addBuffer(*maps[WorldMaps::PositionDepth], "gPositionDepth");
		addBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");
	}

}
