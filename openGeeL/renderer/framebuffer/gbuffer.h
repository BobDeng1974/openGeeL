#ifndef GBUFFER_H
#define GBUFFER_H

#include "framebuffer.h"

namespace geeL {

	struct ScreenInfo {

	public:
		float CTdepth;
		float TRdepth;
		float TLdepth;
		float BRdepth;
		float BLdepth;
	};


	//Special framebuffer that holds various scene render information like colors, normals and positions
	class GBuffer : public FrameBuffer {

	public:
		unsigned int diffuseSpec;
		unsigned int positionDepth;
		unsigned int normalMet;
		ScreenInfo screenInfo;

		GBuffer();

		void init(int width, int height);

		void fill(Drawer& drawer, bool setFBO = false);
		virtual void fill(Drawer& drawer, bool setFBO = false) const;

		//Returns depth of current drawn image
		float getDepth() const;

	private:
		float depthPos;
		unsigned int generateTexture(unsigned int position, unsigned int type);

	};
}

#endif
