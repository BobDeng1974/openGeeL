#ifndef COLORBUFFER_H
#define COLORBUFFER_H

#include <vector>
#include "framebuffer.h"

namespace geeL {

	//A simple framebuffer that can store 2D textures
	class ColorBuffer : public FrameBuffer {

	public:
		ColorBuffer() {}
		virtual ~ColorBuffer();

		//Init color buffer with external textures which will be filled by subsequent drawcalls
		//Note: Memory of render textures won't be managed by this color buffer
		//Note: It is assumed that given textures have same dimensions.
		void init(Resolution resolution, std::vector<RenderTexture*>& colorBuffers);

		//Init color buffer with external texture and subsequent drawcalls will fill it
		//Note: Memory of render texture won't be managed by this color buffer
		void init(Resolution resolution, RenderTexture& texture);

		//Init color buffer and create own render textures with given parameters
		//Note: Memory of these render texture will be managed by this color buffer
		void init(Resolution resolution, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::ClampEdge);

		virtual void initDepth();

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNormal);
		virtual void fill(Drawer& drawer, Clearer clearer = clearNormal);

		const RenderTexture& getTexture(unsigned int position) const;
		virtual void resize(ResolutionScale resolution);
		virtual Resolution getResolution() const;

		virtual std::string toString() const;

	private:
		Resolution resolution;
		std::vector<std::pair<bool, RenderTexture*>> buffers;

		ColorBuffer(const ColorBuffer& other) = delete;
		ColorBuffer& operator= (const ColorBuffer& other) = delete;

	};

}

#endif
