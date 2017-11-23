#include "transform.h"
#include "transformfactory.h"

namespace geeL {

	TransformFactory::TransformFactory(Transform& world) : world(world) {}


	Transform& TransformFactory::CreateTransform() {
		return CreateTransform(world);
	}

	Transform& TransformFactory::CreateTransform(Transform& parent) {
		std::unique_ptr<Transform> child(new Transform());

		return parent.addChild(std::move(child));
	}

	Transform& TransformFactory::CreateTransform(vec3 position, vec3 rotation, vec3 scaling, bool isStatic) {
		return CreateTransform(world, position, rotation, scaling, isStatic);
	}

	Transform& TransformFactory::CreateTransform(Transform& parent, vec3 position, vec3 rotation, vec3 scaling, bool isStatic) {
		std::unique_ptr<Transform> child(new Transform(position, rotation, scaling, isStatic));

		return parent.addChild(std::move(child));
	}

	Transform& TransformFactory::getWorldTransform() {
		return world;
	}

	void TransformFactory::update() {
		world.update();
	}

}