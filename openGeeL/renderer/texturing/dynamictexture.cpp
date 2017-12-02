#include "dynamictexture.h"

namespace geeL {

	DynamicRenderTexture::DynamicRenderTexture(const Camera& camera, Resolution resolution)
		: RenderTarget(std::unique_ptr<Texture>(new Texture2D(resolution, ColorType::RGBA16)))
		, camera(camera) {
	
		buffer.init(resolution, *this);
		buffer.initDepth();
	}


	void DynamicRenderTexture::update(std::function<void(const Camera&)> renderCall) {
		buffer.fill([this, &renderCall]() {
			renderCall(camera);
		});
	}

}