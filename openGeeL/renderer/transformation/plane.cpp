#include <glm.hpp>
#include "plane.h"

using namespace glm;

namespace geeL {
	
	Plane::Plane()
		: point(0.f)
		, normal(0.f) {}
	
	Plane::Plane(const vec3& point, const vec3& normal)
		: point(point)
		, normal(normal) {}

	Plane::Plane(const vec3& a, const vec3& b, const vec3& c) {
		vec3 span1(b - a);
		vec3 span2(c - a);

		point = a;
		normal = normalize(cross(span1, span2));
	}


	float Plane::getDistance(const glm::vec3& point) {
		return dot(normal, point - this->point);
	}

}