#ifndef GBUFFER_H
#define GBUFFER_H

#include "texturing/rendertexture.h"
#include "framebuffer.h"

namespace geeL {

	//Special framebuffer that holds various scene render information like colors, normals and positions
	class GBuffer : public FrameBuffer {

	public:
		GBuffer(Resolution resolution);
		virtual ~GBuffer();

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearAll);

		virtual Resolution getResolution() const;

		const RenderTexture& getDiffuse() const;
		const RenderTexture& getPosition() const;
		const RenderTexture& getNormal() const;
		const RenderTexture& getOcclusionEmissivityRoughnessMetallic() const;
		RenderTexture& getOcclusion() const;

		std::string getFragmentPath() const;
		virtual std::string toString() const;

	private:
		Resolution resolution;
		RenderTexture* diffuse;
		RenderTexture* position;
		RenderTexture* normal;
		RenderTexture* occEmiRoughMet;

		GBuffer(const GBuffer& other) = delete;
		GBuffer& operator= (const GBuffer& other) = delete;

		void init(Resolution resolution);
		void initTextures(Resolution resolution);

	};


	//Framebuffer that can extend gBuffer by information of forward rendered objects
	class ForwardBuffer : public FrameBuffer {

	public:
		ForwardBuffer(GBuffer& gBuffer);
		
		void setTarget(RenderTarget& target);
		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNothing);

		virtual Resolution getResolution() const;

		virtual std::string toString() const;

	private:
		RenderTarget* target;
		GBuffer& gBuffer;

		void init();

	};

}

#endif
