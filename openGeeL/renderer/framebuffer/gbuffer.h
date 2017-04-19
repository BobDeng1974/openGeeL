#ifndef GBUFFER_H
#define GBUFFER_H

#include "../utility/screeninfo.h"
#include "../texturing/rendertexture.h"
#include "framebuffer.h"

namespace geeL {

	//Special framebuffer that holds various scene render information like colors, normals and positions
	class GBuffer : public FrameBuffer {

	public:
		ScreenInfo screenInfo;

		GBuffer();
		~GBuffer();

		void init(int width, int height);

		virtual void fill(std::function<void()> drawCall);

		//Returns depth of current drawn image
		float getDepth() const;

		const RenderTexture& getDiffuseSpecular() const;
		const RenderTexture& getPositionDepth() const;
		const RenderTexture& getNormalMetallic() const;

	private:
		RenderTexture diffuseSpec;
		RenderTexture positionDepth;
		RenderTexture normalMet;
		float depthPos;

	};
}

#endif
