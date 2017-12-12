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


	//Basic interface for all classes that need to  
	//add information to GL context during runtime
	class GLStructure {

	public:
		virtual void initGL() = 0;
		virtual void clearGL() = 0;

	};

}

#endif
