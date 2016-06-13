#include "transform.h"
#include "transformfactory.h"

namespace geeL {

	Transform& TransformFactory::CreateTransform() {
		return CreateTransform(world);
	}

	Transform& TransformFactory::CreateTransform(Transform& parent) {
		return parent.AddChild(Transform(parent));
	}

	Transform& TransformFactory::CreateTransform(vec3 position, vec3 rotation, vec3 scaling) {
		return CreateTransform(world, position, rotation, scaling);
	}

	Transform& TransformFactory::CreateTransform(Transform& parent, vec3 position, vec3 rotation, vec3 scaling) {
		return parent.AddChild(Transform(position, rotation, scaling, &parent));
	}

}