#ifndef GBUFFER_H
#define GBUFFER_H

#include "framebuffer.h"

namespace geeL {

	class ITextureProvider;
	class RenderTarget;


	//Special framebuffer that holds various scene render information like colors, normals and positions
	class GBuffer : public FrameBuffer {

	public:
		GBuffer(ITextureProvider& provider);

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearAll);

		std::string getFragmentPath() const;
		virtual std::string toString() const;
		virtual Resolution getResolution() const;

	private:
		ITextureProvider& provider;

		GBuffer(const GBuffer& other) = delete;
		GBuffer& operator= (const GBuffer& other) = delete;

		void init();
		void initTextures(Resolution resolution);

	};


	//Framebuffer that can extend gBuffer by information of forward rendered objects
	class ForwardBuffer : public FrameBuffer {

	public:
		ForwardBuffer(GBuffer& gBuffer, ITextureProvider& provider);
		
		void setTarget(RenderTarget& target);
		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNothing);

		virtual Resolution getResolution() const;

		virtual std::string toString() const;

	private:
		GBuffer& gBuffer;
		ITextureProvider& provider;
		RenderTarget* target;

		void init();

	};

}

#endif
