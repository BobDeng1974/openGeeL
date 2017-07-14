#ifndef CUBEBUFFER_H
#define CUBEBUFFER_H

#include "framebuffer.h"

namespace geeL {

	class TextureCube;

	class CubeBuffer : public FrameBuffer {

	public:
		CubeBuffer();
		~CubeBuffer();

		void init(const TextureCube& texture);

		virtual void fill(std::function<void()> drawCall) const;
		void fill(std::function<void(unsigned int)> drawCall, unsigned int mipLevel = 0);

		void resize(unsigned int width, unsigned int height);

		virtual std::string toString() const;

	private:
		const TextureCube* texture;
		unsigned int rbo;

		CubeBuffer(const CubeBuffer& other) = delete;
		CubeBuffer& operator= (const CubeBuffer& other) = delete;

	};
}

#endif
