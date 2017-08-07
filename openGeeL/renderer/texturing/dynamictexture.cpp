#include "dynamictexture.h"

namespace geeL {

	DynamicRenderTexture::DynamicRenderTexture(const Camera& camera, Resolution resolution, 
		std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall)
			: RenderTexture(resolution, ColorType::RGB16, WrapMode::Repeat), 
				camera(camera), renderCall(renderCall) {
	
		buffer.init(resolution, *this);
		buffer.initDepth();
	}


	void DynamicRenderTexture::update() {
		renderCall(camera, buffer);
	}


}