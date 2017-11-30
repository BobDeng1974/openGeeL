#ifndef CUBEBUFFER_H
#define CUBEBUFFER_H

#include "framebuffer.h"

namespace geeL {

	class TextureCube;

	class CubeBuffer : public FrameBuffer {

	public:
		CubeBuffer();

		void init(const TextureCube& texture);

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNormal);
		void fill(std::function<void(unsigned int)> drawCall, unsigned int mipLevel = 0, Clearer clearer = clearNormal);

		virtual Resolution getResolution() const;
		void resize(ResolutionScale resolution);

		virtual std::string toString() const;

	private:
		Resolution resolution;
		const TextureCube* texture;

		CubeBuffer(const CubeBuffer& other) = delete;
		CubeBuffer& operator= (const CubeBuffer& other) = delete;

	};
}

#endif
