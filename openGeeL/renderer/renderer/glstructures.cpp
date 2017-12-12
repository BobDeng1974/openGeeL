#include "glstructures.h"

namespace geeL {

	void Drawer::setParent(DynamicBuffer& buffer) {
		parentBuffer = &buffer;
	}

	const DynamicBuffer* const Drawer::getParentBuffer() const {
		return parentBuffer;
	}

	DynamicBuffer* const Drawer::getParentBuffer() {
		return parentBuffer;
	}

}