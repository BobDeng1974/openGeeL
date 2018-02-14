#include <algorithm>
#include <limits>
#include "transformation/transform.h"
#include "viewfrustum.h"
#include "boundingbox.h"

using namespace glm;

namespace geeL {

	AABoundingBox::AABoundingBox()
		: min(vec3(std::numeric_limits<float>::max()))
		, max(vec3(std::numeric_limits<float>::min())) {}

	AABoundingBox::AABoundingBox(const glm::vec3& min, const glm::vec3& max)
		: min(min)
		, max(max) {}

	AABoundingBox::AABoundingBox(const AABoundingBox& other)
		: min(other.min)
		, max(other.max) {}


	void AABoundingBox::extend(const glm::vec3& point) {
		min.x = std::min(min.x, point.x);
		min.y = std::min(min.y, point.y);
		min.z = std::min(min.z, point.z);

		max.x = std::max(max.x, point.x);
		max.y = std::max(max.y, point.y);
		max.z = std::max(max.z, point.z);

		onChange();
	}

	void AABoundingBox::extend(const AABoundingBox& box) {
		extend(box.getMin());
		extend(box.getMax());
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

	float AABoundingBox::distanceCenter(const glm::vec3& point) const {
		return length(getCenter() - point);
	}

	const glm::vec3 AABoundingBox::getCenter() const {
		return min + 0.5f * (max - min);
	}

	const glm::vec3 AABoundingBox::getDiagonal() const {
		return max - min;
	}

	const glm::vec3 AABoundingBox::getSize() const {
		return glm::abs(max - min);
	}

	std::string AABoundingBox::toString() const {
		return "AABB: (" + std::to_string(min.x) + ", " + std::to_string(min.y) + ", " + std::to_string(min.z) + ") - ("
			+ std::to_string(max.x) + ", " + std::to_string(max.y) + ", " + std::to_string(max.z) + ")";
	}

	bool AABoundingBox::operator==(const AABoundingBox& other) const {
		return (min == other.min) && (max == other.max);
	}

	bool AABoundingBox::operator!=(const AABoundingBox& other) const {
		return (min != other.min) || (max != other.max);
	}

	bool AABoundingBox::contains(const glm::vec3& point) const {
		return (min.x <= point.x) && (max.x >= point.x)
			&& (min.y <= point.y) && (max.y >= point.y)
			&& (min.z <= point.z) && (max.z >= point.z);
	}

	bool AABoundingBox::contains(const AABoundingBox& box) const {
		return contains(box.getMin()) && contains(box.getMax());
	}

	void AABoundingBox::reset() {
		min = vec3(std::numeric_limits<float>::max());
		max = vec3(std::numeric_limits<float>::min());
	}

	IntersectionType AABoundingBox::intersect(const ViewFrustum& frustum) const {
		IntersectionType type = IntersectionType::Inside;

		for (short i = 0; i < 6; i++) {
			const Plane& plane = frustum.getPlane(i);

			vec3 p = getMax(plane.getNormal());
			if (plane.getDistance(p) < 0.f)
				return IntersectionType::Outside;

			p = getMin(plane.getNormal());
			if (plane.getDistance(p) < 0.f)
				type = IntersectionType::Intersect;
		}

		return type;
	}



	TransformableBoundingBox::TransformableBoundingBox(const AABoundingBox& localBox, Transform& transform)
		: AABoundingBox(localBox)
		, localBox(localBox)
		, transform(transform) {

		updateGlobal();

		transform.addChangeListener([this](const Transform& transform) {
			updateGlobal();
		});
	}

	const AABoundingBox& TransformableBoundingBox::getLocalBox() const {
		return localBox;
	}

	void TransformableBoundingBox::setLocalBox(const AABoundingBox& localBox) {
		this->localBox = localBox;
		updateGlobal();
	}

	void TransformableBoundingBox::updateGlobal() {
		vec3 tmin(transform.getMatrix() * vec4(localBox.getMin(), 1.f));
		vec3 tmax(transform.getMatrix() * vec4(localBox.getMax(), 1.f));

		min = vec3(std::min(tmin.x, tmax.x), std::min(tmin.y, tmax.y), std::min(tmin.z, tmax.z));
		max = vec3(std::max(tmin.x, tmax.x), std::max(tmin.y, tmax.y), std::max(tmin.z, tmax.z));
	}

}
