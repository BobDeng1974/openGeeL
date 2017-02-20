#ifndef GBUFFER_H
#define GBUFFER_H

namespace geeL {

	class Drawer;


	struct ScreenInfo {

	public:
		float CTdepth;
		float TRdepth;
		float TLdepth;
		float BRdepth;
		float BLdepth;
	};


	class GBuffer {

	public:
		unsigned int fbo;
		unsigned int diffuseSpec;
		unsigned int positionDepth;
		unsigned int normalMet;
		ScreenInfo screenInfo;
		int width;
		int height;

		GBuffer();

		void init(int width, int height);
		void fill(Drawer& drawer);

		//Returns depth of current drawn image
		float getDepth() const;

	private:
		float depthPos;
		unsigned int generateTexture(unsigned int position, unsigned int type);

	};
}

#endif
