#ifndef COLORBUFFER_H
#define COLORBUFFER_H

#include <vector>
#include "framebuffer.h"

namespace geeL {

	//A simple framebuffer that can store 2D textures
	class ColorBuffer : public FrameBuffer {

	public:
		ColorBuffer() {}
		~ColorBuffer();

		void init(Resolution resolution, std::vector<RenderTexture*>&& colorBuffers);
		void init(Resolution resolution, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::ClampEdge);

		virtual void initDepth();

		virtual void fill(std::function<void()> drawCall);
		virtual void fill(Drawer& drawer);

		const RenderTexture& getTexture(unsigned int position = 0) const;
		virtual void resize(ResolutionScale resolution);

		virtual std::string toString() const;

	private:
		unsigned int rbo;
		std::vector<RenderTexture*> buffers;

		ColorBuffer(const ColorBuffer& other) = delete;
		ColorBuffer& operator= (const ColorBuffer& other) = delete;

	};

}

#endif
