#ifndef DYNAMICRENDERTEXTURE_H
#define DYNAMICRENDERTEXTURE_H

#include <functional>
#include "framebuffer/colorbuffer.h"
#include "rendertexture.h"

namespace geeL {

	class Camera;
	class FrameBuffer;
	class ColorBuffer;


	//Render texture that can render scene from given camera's point of view.
	class DynamicRenderTexture : public RenderTexture {

	public:
		DynamicRenderTexture(const Camera& camera, Resolution resolution, 
			std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall);

		void update();

	private:
		std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall;
		const Camera& camera;
		ColorBuffer buffer;

	};

}

#endif
