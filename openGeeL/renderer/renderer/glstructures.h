#ifndef GLSTRUCTURES_H
#define GLSTRUCTURES_H

#include <string>
#include "memory/memoryobject.h"

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

		virtual std::string toString() const = 0;


		template<typename Structure, typename ...StructureArgs>
		//Creates an GL structure that get automatically initialized/ disposed
		//Important: Using this in a thread that doesn't hold GL context will
		//result in undefined behaviour
		static del_unique_ptr<Structure> create(StructureArgs&& ...args);

	};


	template<typename Structure, typename ...StructureArgs>
	inline del_unique_ptr<Structure> GLStructure::create(StructureArgs && ...args) {
		Structure* structure = new Structure(std::forward<StructureArgs>(args)...);
		structure->initGL();

		return del_unique_ptr<Structure>(structure, [](Structure* s) {
			s->clearGL();
			delete s;
		});
	}

}

#endif
