#ifndef GLSTRUCTURES_H
#define GLSTRUCTURES_H

namespace geeL {

	class DynamicBuffer;

	//Basic interface for all rendering classes
	class Drawer {

	public:
		virtual void draw() = 0;

		void setParent(DynamicBuffer& buffer);
		const DynamicBuffer* const getParentBuffer() const;
		DynamicBuffer* const getParentBuffer();

	protected:
		DynamicBuffer* parentBuffer = nullptr;

	};


}

#endif
