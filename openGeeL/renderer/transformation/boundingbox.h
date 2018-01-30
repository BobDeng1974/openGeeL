#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <vec3.hpp>

namespace geeL {

	enum class IntersectionType;
	class ViewFrustum;


	class AABoundingBox {

	public:
		AABoundingBox();

		//Update bounding box to include given point
		void update(const glm::vec3& point);

		//Get minimum point in world space
		const glm::vec3& getMin() const;

		//Get maximum point in world space
		const glm::vec3& getMax() const;

		//Get minimum point relative to given normal
		glm::vec3 getMin(const glm::vec3& normal) const;

		//Get maximum point relative to given normal
		glm::vec3 getMax(const glm::vec3& normal) const;


		IntersectionType intersect(const ViewFrustum& frustum) const;

	private:
		glm::vec3 min, max;

	};

}

#endif
