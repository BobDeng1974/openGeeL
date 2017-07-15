#include <iostream>
#include "../renderer.h"
#include "stackbuffer.h"

using namespace std;

namespace geeL {

	StackBuffer::StackBuffer() {
		reset();
	}

	StackBuffer::~StackBuffer() {
		//remove();

	}


	void StackBuffer::init(unsigned int width, unsigned int height, ColorType colorType, 
		FilterMode filterMode, WrapMode wrapMode) {
		
		first.init(width, height, colorType, filterMode, wrapMode);
		second.init(width, height, colorType, filterMode, wrapMode);
	}

	void StackBuffer::bind() const {
		current->bind();
	}

	void StackBuffer::reset() {
		current = &first;
	}

	void StackBuffer::initDepth() {
		first.initDepth();
	}

	
	void StackBuffer::resize(Resolution resolution) {
		current->resize(resolution);
	}

	void StackBuffer::copyDepth(const FrameBuffer& buffer) const {
		current->copyDepth(buffer);
	}
	

	void StackBuffer::fill(std::function<void()> drawCall) {
		current->fill(drawCall);
		swap();
	}

	void StackBuffer::fill(Drawer & drawer) {
		current->fill(drawer);
		swap();
	}

	
	const RenderTexture& StackBuffer::getTexture(unsigned int position) const {
		if (position == 0)
			return first.getTexture(0);
		else
			return second.getTexture(0);
	}

	void StackBuffer::resetSize() const {
		current->resetSize();
	}

	unsigned int StackBuffer::getWidth() const {
		return current->getWidth();
	}

	unsigned int StackBuffer::getHeight() const {
		return current->getHeight();
	}

	std::string StackBuffer::toString() const {
		return "Stack Buffer";
	}

	void StackBuffer::swap() {
		if (current == &first)
			current = &second;
		else
			current = &first;

	}



}