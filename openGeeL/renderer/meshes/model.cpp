#include "memory/memory.h"
#include "appmanager.h"
#include "model.h"

using namespace geeL::memory;

namespace geeL {
	
	void Model::initGL() {
		iterateMeshes([](Mesh& mesh) {
			mesh.initGL();
		});
	}
	
	void Model::clearGL() {
		iterateMeshes([](Mesh& mesh) {
			mesh.clearGL();
		});
	}
	
	std::string Model::toString() const {
		return path;
	}

	const std::string& Model::getPath() const {
		return path;
	}

	void* Model::operator new(size_t size) {
		Memory& memory = ApplicationManager::getCurrentMemory();
		return memory.allocate(size);
	}

	void Model::operator delete(void* pointer) {
		Memory& memory = ApplicationManager::getCurrentMemory();
		memory.deallocate(pointer);
	}


}