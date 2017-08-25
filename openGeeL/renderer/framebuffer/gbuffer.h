#ifndef GBUFFER_H
#define GBUFFER_H

#include "utility/screeninfo.h"
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
		ScreenInfo screenInfo;

		GBuffer(Resolution resolution, GBufferContent content = GBufferContent::Default);
		virtual ~GBuffer();

		virtual void fill(std::function<void()> drawCall);

		//Returns depth of current drawn image
		float getDepth() const;

		const RenderTexture& getDiffuse() const;
		const RenderTexture& getPositionRoughness() const;
		const RenderTexture& getNormalMetallic() const;
		const RenderTexture& getEmissivity() const;

		std::string getFragmentPath() const;
		virtual std::string toString() const;

	private:
		RenderTexture* diffuse;
		RenderTexture* positionRough;
		RenderTexture* normalMet;
		RenderTexture* emissivity;
		GBufferContent content;
		float depthPos;

		GBuffer(const GBuffer& other) = delete;
		GBuffer& operator= (const GBuffer& other) = delete;

		void init(Resolution resolution);
		void initTextures(Resolution resolution);

	};
}

#endif
