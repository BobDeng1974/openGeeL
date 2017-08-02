#include "transform.h"
#include "threadedtransform.h"
#include "transformfactory.h"

namespace geeL {

	TransformFactory::TransformFactory(Transform& world) : world(world) {}


	Transform& TransformFactory::CreateTransform() {
		return CreateTransform(world);
	}

	Transform& TransformFactory::CreateTransform(Transform& parent) {
		return parent.AddChild(new Transform());
	}

	Transform& TransformFactory::CreateTransform(vec3 position, vec3 rotation, vec3 scaling, bool isStatic) {
		return CreateTransform(world, position, rotation, scaling, isStatic);
	}

	Transform& TransformFactory::CreateTransform(Transform& parent, vec3 position, vec3 rotation, vec3 scaling, bool isStatic) {
		//return parent.AddChild(new Transform(position, rotation, scaling, isStatic));
		return parent.AddChild(new ThreadedTransform(position, rotation, scaling, isStatic));
	}

	Transform& TransformFactory::getWorldTransform() {
		return world;
	}

	void TransformFactory::update() {
		world.update();
	}

}