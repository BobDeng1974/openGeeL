#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <string>
#include <vec3.hpp>
#include "utility/listener.h"

namespace geeL {

	enum class IntersectionType;
	class Transform;
	class ViewFrustum;


	class AABoundingBox : public ChangeActuator<AABoundingBox> {

	public:
		AABoundingBox();
		AABoundingBox(const glm::vec3& min, const glm::vec3& max);
		AABoundingBox(const AABoundingBox& other);
		virtual ~AABoundingBox() {}


		//Update local bounding box to include given point
		void update(const glm::vec3& point);

		//Update local bounding box to include given bounding box
		void update(const AABoundingBox& box);

		

		IntersectionType intersect(const ViewFrustum& frustum) const;


		//Get minimum point in world space
		const glm::vec3& getMin() const;

		//Get maximum point in world space
		const glm::vec3& getMax() const;

		//Get minimum point relative to given normal
		glm::vec3 getMin(const glm::vec3& normal) const;

		//Get maximum point relative to given normal
		glm::vec3 getMax(const glm::vec3& normal) const;

		std::string toString() const;

	protected:
		glm::vec3 min, max;

	};


	class TransformableBoundingBox : public AABoundingBox {

	public:
		TransformableBoundingBox(AABoundingBox& localBox, Transform& transform);

	private:
		const AABoundingBox& localBox; 
		Transform& transform;


		void updateGlobal();

	};


}

#endif
