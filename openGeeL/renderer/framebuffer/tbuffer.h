#ifndef TBUFFER_H
#define TBUFFER_H

#include "postprocessing/postprocessing.h"
#include "gbuffer.h"

namespace geeL {

	class StackBuffer;


	//Framebuffer that draws transparent objects and extends gBuffer with their information
	class TransparentBuffer : public FrameBuffer {

	public:
		TransparentBuffer(GBuffer& gBuffer, StackBuffer& stackBuffer);
		virtual ~TransparentBuffer();

		void init(RenderTexture& colorTexture);
		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearAll);
		void composite();

		virtual std::string toString() const;

	private:
		GBuffer& gBuffer;
		StackBuffer& stackBuffer;

		RenderTexture* accumulationTexture;
		RenderTexture* revealageTexture;
		RenderTexture* compositionTexture;

		PostProcessingEffectFS tComp;

	};

}

#endif

