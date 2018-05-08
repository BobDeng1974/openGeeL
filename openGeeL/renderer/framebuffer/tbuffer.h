#ifndef TBUFFER_H
#define TBUFFER_H

#define ORDER_INDEPENDENT_TRANSPARENCY 0


#if ORDER_INDEPENDENT_TRANSPARENCY

#include "postprocessing/postprocessing.h"
#include "gbuffer.h"

namespace geeL {

	class DynamicBuffer;

	//Framebuffer that draws transparent objects and extends gBuffer with their information
	//Implements Weighted, Order-Independent Transparency according to
	//http://casual-effects.blogspot.de/2015/03/implemented-weighted-blended-order.html
	class TransparentOIDBuffer : public FrameBuffer {

	public:
		TransparentOIDBuffer(GBuffer& gBuffer, ITextureProvider& provider, DynamicBuffer& compBuffer);
		virtual ~TransparentOIDBuffer();

		void init(RenderTexture& colorTexture);
		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNothing);
		void composite();

		virtual Resolution getResolution() const;

		virtual std::string toString() const;

	private:
		GBuffer& gBuffer;
		ITextureProvider& provider;
		DynamicBuffer& compBuffer;

		RenderTexture* accumulationTexture;
		RenderTexture* revealageTexture;
		RenderTexture* compositionTexture;

		PostProcessingEffectFS tComp;

	};

}

#endif

#endif

