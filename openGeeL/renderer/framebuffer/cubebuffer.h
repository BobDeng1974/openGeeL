#ifndef CUBEBUFFER_H
#define CUBEBUFFER_H

#include "framebuffer.h"

namespace geeL {

	class TextureCube;

	class CubeBuffer : public FrameBuffer {

	public:
		CubeBuffer();

		void init(const TextureCube& texture);

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearAll);
		void fill(std::function<void(unsigned int)> drawCall, unsigned int mipLevel = 0, Clearer clearer = clearAll);

		void resize(ResolutionScale resolution);

		virtual std::string toString() const;

	private:
		const TextureCube* texture;
		unsigned int rbo;

		CubeBuffer(const CubeBuffer& other) = delete;
		CubeBuffer& operator= (const CubeBuffer& other) = delete;

	};
}

#endif
