#ifndef DYNAMICRENDERTEXTURE_H
#define DYNAMICRENDERTEXTURE_H

#include <functional>
#include "framebuffer/colorbuffer.h"
#include "framebuffer/rendertarget.h"

namespace geeL {

	class Camera;
	class FrameBuffer;
	class ColorBuffer;


	//Render texture that can render scene from given camera's point of view.
	class DynamicRenderTexture : public RenderTarget {

	public:
		DynamicRenderTexture(const Camera& camera, Resolution resolution);

		void update(std::function<void(const Camera&)> renderCall);

	private:
		const Camera& camera;
		ColorBuffer buffer;

	};

}

#endif
