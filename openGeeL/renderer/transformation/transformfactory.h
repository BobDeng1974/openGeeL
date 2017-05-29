#ifndef TRANSFORMFACTORY_H
#define TRANSFORMFACTORY_H

#include <vec3.hpp>

namespace geeL {

	class Transform;

	class TransformFactory {

	public:
		TransformFactory(Transform& world);

		//Creates and and returns an empty transform with
		//world transform as parent
		Transform& CreateTransform();

		//Creates and and returns an empty transform with
		//committed transform as parent
		Transform& CreateTransform(Transform& parent);

		//Creates and and returns a transform with
		//world transform as parent
		Transform& CreateTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scaling, bool isStatic = false);

		//Creates and and returns a transform with
		//committed transform as parent
		Transform& CreateTransform(Transform& parent, 
			glm::vec3 position, glm::vec3 rotation, glm::vec3 scaling, bool isStatic = false);

		Transform& getWorldTransform();

		//Update transformation tree with all made to internal positions,
		//rotations and scalings
		void update();

	private:
		Transform& world;

	};
}

#endif
