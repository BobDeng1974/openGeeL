#include "framebuffer.h"
#include "rendertarget.h"

namespace geeL {

	RenderTarget::RenderTarget() : parent(nullptr) {}

	void RenderTarget::assignTo(const IFrameBuffer& buffer, unsigned int position, bool bindFB) {
		if (bindFB) buffer.bind();
		parent = &buffer;

		assignInner(position);
		if (bindFB) buffer.unbind();

	}

	bool RenderTarget::assignToo(const IFrameBuffer& buffer, unsigned int position, bool bindFB) const {
		if (parent != nullptr) {
			if (bindFB) buffer.bind();
			assignInner(position);
			if (bindFB) buffer.unbind();

			return true;
		}

		return false;
	}

	bool RenderTarget::isAssigned() const {
		return parent != nullptr;
	}


	void LayeredTarget::setRenderResolution() const {
		targets.front()->setRenderResolution();
	}

	const Resolution& LayeredTarget::getResolution() const {
		return targets.front()->getResolution();
	}

	void LayeredTarget::assignInner(unsigned int position) const {

		unsigned int pos = position;
		for (auto it(targets.begin()); it != targets.end(); it++) {
			RenderTarget& current = **it;
			current.assignInner(pos);

			pos++;
		}

	}



}