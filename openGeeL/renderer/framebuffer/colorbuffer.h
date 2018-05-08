#ifndef COLORBUFFER_H
#define COLORBUFFER_H

#include <vector>
#include "framebuffer.h"

namespace geeL {

	//A simple framebuffer that can store 2D textures
	class ColorBuffer : public FrameBuffer {

	public:
		ColorBuffer() {}

		//Init color buffer with external textures which will be filled by subsequent drawcalls
		//Note: Memory of render textures won't be managed by this color buffer
		//Note: It is assumed that given textures have same dimensions.
		void init(Resolution resolution, std::vector<RenderTarget*>& colorBuffers);

		//Init color buffer with external texture and subsequent drawcalls will fill it
		//Note: Memory of render texture won't be managed by this color buffer
		void init(Resolution resolution, RenderTarget& texture);

		virtual void initDepth();

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNormal);
		virtual void fill(Drawer& drawer, Clearer clearer = clearNormal);

		const RenderTarget& getTexture(unsigned int position) const;
		virtual Resolution getResolution() const;

		virtual std::string toString() const;

	private:
		Resolution resolution;
		std::vector<RenderTarget*> buffers;

		ColorBuffer(const ColorBuffer& other) = delete;
		ColorBuffer& operator= (const ColorBuffer& other) = delete;

	};

}

#endif
