#ifndef GEOMETRICPLANE_H
#define GEOMETRICPLANE_H

#include <vec3.hpp>

namespace geeL {

	class Plane {

	public:
		Plane();
		Plane(const glm::vec3& point, const glm::vec3& normal);
		Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);


		float getDistance(const glm::vec3& point);

	private:
		glm::vec3 point, normal;

	};

}

#endif
