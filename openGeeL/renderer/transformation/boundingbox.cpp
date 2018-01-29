#include <algorithm>
#include <limits>
#include "boundingbox.h"

using namespace glm;

namespace geeL {

	AABoundingBox::AABoundingBox()
		: min(vec3(std::numeric_limits<float>::max()))
		, max(vec3(std::numeric_limits<float>::min())) {}


	void AABoundingBox::update(const glm::vec3& point) {
		min.x = std::min(min.x, point.x);
		min.y = std::min(min.y, point.y);
		min.z = std::min(min.z, point.z);

		max.x = std::max(max.x, point.x);
		max.y = std::max(max.y, point.y);
		max.z = std::max(max.z, point.z);
	}

	const glm::vec3& AABoundingBox::getMin() const {
		return min;
	}

	const glm::vec3& AABoundingBox::getMax() const {
		return max;
	}

	glm::vec3 AABoundingBox::getMin(const glm::vec3& normal) const {
		vec3 p(max);

		if (normal.x >= 0)
			p.x = min.x;
		if (normal.y >= 0)
			p.y = min.y;
		if (normal.z >= 0)
			p.z = min.z;

		return p;
	}

	glm::vec3 AABoundingBox::getMax(const glm::vec3& normal) const {
		vec3 p(min);

		if (normal.x >= 0)
			p.x = max.x;
		if (normal.y >= 0)
			p.y = max.y;
		if (normal.z >= 0)
			p.z = max.z;

		return p;
	}

}