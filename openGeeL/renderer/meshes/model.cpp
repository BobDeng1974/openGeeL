#include "animation/animation.h"
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

	const AABoundingBox& Model::getBoundingBox() const {
		return aabb;
	}

	void* Model::operator new(size_t size) {
		Memory& memory = ApplicationManager::getCurrentMemory();
		return memory.allocate(size);
	}

	void Model::operator delete(void* pointer) {
		Memory& memory = ApplicationManager::getCurrentMemory();
		memory.deallocate(pointer);
	}

	
	void SkinnedModel::addAnimation(std::unique_ptr<AnimationMemory> animation) {
		//TODO: Adjust AABB to surround model in every possible animation frame
		AnimationContainer::addAnimation(std::move(animation));
	}
	
}