#include "bufferutil.h"
#include "framebuffer.h"
#include "rendertarget.h"
#include <iostream>

namespace geeL {

	ARenderTarget::ARenderTarget() : parent(nullptr) {}

	void ARenderTarget::assignTo(const IFrameBuffer& buffer, unsigned int position, bool bindFB) {
		if (bindFB) buffer.bind();
		parent = &buffer;

		assign(position);
		if (bindFB) buffer.unbind();

	}

	bool ARenderTarget::assignToo(const IFrameBuffer& buffer, unsigned int position, bool bindFB) const {
		if (parent != nullptr) {
			if (bindFB) buffer.bind();
			assign(position);
			if (bindFB) buffer.unbind();

			return true;
		}

		return false;
	}

	bool ARenderTarget::isAssigned() const {
		return parent != nullptr;
	}


	void LayeredTarget::setRenderResolution() const {
		targets.front()->setRenderResolution();
	}

	Resolution LayeredTarget::getRenderResolution() const {
		return targets.front()->getRenderResolution();
	}

	unsigned int LayeredTarget::getSize() const {
		return unsigned int(targets.size());
	}

	void LayeredTarget::assign(unsigned int position) const {

		unsigned int size = 0;
		for (auto it(targets.begin()); it != targets.end(); it++) {
			ARenderTarget& current = **it;
			current.assign(position + size);

			size++;
		}
	}



}