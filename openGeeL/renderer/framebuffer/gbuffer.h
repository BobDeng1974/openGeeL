#ifndef GBUFFER_H
#define GBUFFER_H

#include "texturing/rendertexture.h"
#include "framebuffer.h"

namespace geeL {

	enum class GBufferContent {
		Default,
		DefaultEmissive
	};


	//Special framebuffer that holds various scene render information like colors, normals and positions
	class GBuffer : public FrameBuffer {

	public:
		GBuffer(Resolution resolution, GBufferContent content = GBufferContent::Default);
		virtual ~GBuffer();

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearAll);

		//Returns depth of current drawn image
		float getDepth() const;
		virtual Resolution getResolution() const;

		const RenderTexture& getDiffuse() const;
		const RenderTexture& getPositionRoughness() const;
		const RenderTexture& getNormalMetallic() const;

		const RenderTexture* getEmissivity() const;
		const RenderTexture* getOcclusion() const;

		//Request occlusion texture of this buffer. Creates a 
		//new one with given scale, if it doesn't exist yet
		RenderTexture& requestOcclusion(const ResolutionScale& scale = 1.f);

		std::string getFragmentPath() const;
		virtual std::string toString() const;

	private:
		Resolution resolution;
		RenderTexture* diffuse;
		RenderTexture* positionRough;
		RenderTexture* normalMet;
		RenderTexture* emissivity;
		RenderTexture* occlusion;
		GBufferContent content;
		float depthPos;

		GBuffer(const GBuffer& other) = delete;
		GBuffer& operator= (const GBuffer& other) = delete;

		void init(Resolution resolution);
		void initTextures(Resolution resolution);

	};


	//Framebuffer that can extend gBuffer by information of forward rendered objects
	class ForwardBuffer : public FrameBuffer {

	public:
		ForwardBuffer(GBuffer& gBuffer);
		
		void setTarget(ARenderTarget& target);
		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNothing);

		virtual Resolution getResolution() const;

		virtual std::string toString() const;

	private:
		ARenderTarget* target;
		GBuffer& gBuffer;

		void init();

	};

}

#endif
