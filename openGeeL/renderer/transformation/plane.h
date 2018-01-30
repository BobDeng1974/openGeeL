#ifndef GEOMETRICPLANE_H
#define GEOMETRICPLANE_H

#include <vec3.hpp>

namespace geeL {

	class Plane {

	public:
		Plane();
		Plane(const glm::vec3& point, const glm::vec3& normal);
		Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);


		const glm::vec3& getPoint() const;
		const glm::vec3& getNormal() const;
		float getDistance(const glm::vec3& point) const;

	private:
		glm::vec3 point, normal;

	};

}

#endif
