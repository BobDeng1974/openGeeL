#ifndef BOUNDEDOBJECt_H
#define BOUNDEDOBJECt_H

#include "boundingbox.h"

namespace geeL {

	class BoundedObject {

	public:
		~BoundedObject() {}

		const AABoundingBox& getBoundingBox() const;

	protected:
		AABoundingBox aabb;

	};

	inline const AABoundingBox& BoundedObject::getBoundingBox() const {
		return aabb;
	}

}

#endif