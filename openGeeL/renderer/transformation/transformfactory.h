#ifndef TRANSFORMFACTORY_H
#define TRANSFORMFACTORY_H

#include <vec3.hpp>

using glm::vec3;

namespace geeL {

	class Transform;

	class TransformFactory {

	public:

		TransformFactory(Transform world) : world(world) {}

		//Creates and and returns an empty transform with
		//world transform as parent
		Transform& CreateTransform();

		//Creates and and returns an empty transform with
		//committed transform as parent
		Transform& CreateTransform(Transform& parent);

		//Creates and and returns a transform with
		//world transform as parent
		Transform& CreateTransform(vec3 position, vec3 rotation, vec3 scaling);

		//Creates and and returns a transform with
		//committed transform as parent
		Transform& CreateTransform(Transform& parent, 
			vec3 position, vec3 rotation, vec3 scaling);

	private:
		Transform world;

	};
}

#endif
