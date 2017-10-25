#include "dynamictexture.h"

namespace geeL {

	DynamicRenderTexture::DynamicRenderTexture(const Camera& camera, Resolution resolution)
		: RenderTexture(resolution, ColorType::RGBA16, WrapMode::Repeat), camera(camera) {
	
		buffer.init(resolution, *this);
		buffer.initDepth();
	}


	void DynamicRenderTexture::update(std::function<void(const Camera&)> renderCall) {
		buffer.fill([this, &renderCall]() {
			renderCall(camera);
		});
	}

}