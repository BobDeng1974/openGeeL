#ifndef CUBEBUFFER_H
#define CUBEBUFFER_H

#include "framebuffer.h"

namespace geeL {

	class CubeBuffer : public FrameBuffer {

	public:
		CubeBuffer();
		CubeBuffer(const CubeBuffer& buffer);
		~CubeBuffer();

		void init(unsigned int resolution, unsigned int textureID);

		virtual void fill(std::function<void()> drawCall);
		void fill(std::function<void(unsigned int)> drawCall, unsigned int mipLevel = 0);

		void resize(unsigned int width, unsigned int height);

		virtual std::string toString() const;

	private:
		unsigned int textureID, rbo;

	};


}

#endif
