#include "transform.h"
#include "transformfactory.h"

namespace geeL {

	TransformFactory::TransformFactory(Transform& world) : world(world) {}


	Transform& TransformFactory::CreateTransform() {
		return CreateTransform(world);
	}

	Transform& TransformFactory::CreateTransform(Transform& parent) {
		return parent.AddChild(new Transform(parent));
	}

	Transform& TransformFactory::CreateTransform(vec3 position, vec3 rotation, vec3 scaling) {
		return CreateTransform(world, position, rotation, scaling);
	}

	Transform& TransformFactory::CreateTransform(Transform& parent, vec3 position, vec3 rotation, vec3 scaling) {
		return parent.AddChild(new Transform(position, rotation, scaling, &parent));
	}

	Transform& TransformFactory::getWorldTransform() {
		return world;
	}

	void TransformFactory::update() {
		world.update();
	}

}