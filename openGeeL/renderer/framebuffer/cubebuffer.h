#ifndef CUBEBUFFER_H
#define CUBEBUFFER_H

#include "framebuffer.h"

namespace geeL {


	class CubeBuffer : public FrameBuffer {

	public:
		void init(unsigned int resolution, unsigned int textureID);

		virtual void fill(std::function<void()> drawCall);

	private:
		unsigned int textureID;

	};


}

#endif
