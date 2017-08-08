#include "dynamictexture.h"

namespace geeL {

	DynamicRenderTexture::DynamicRenderTexture(const Camera& camera, Resolution resolution, 
		std::function<void(const Camera&)> renderCall)
			: RenderTexture(resolution, ColorType::RGBA16, WrapMode::Repeat), 
				camera(camera), renderCall(renderCall) {
	
		buffer.init(resolution, *this);
		buffer.initDepth();
	}


	void DynamicRenderTexture::update() {
		buffer.fill([&]() {
			renderCall(camera);
		});
	}


}