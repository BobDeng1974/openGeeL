#ifndef BOUNDEDOBJECt_H
#define BOUNDEDOBJECt_H

#include "boundingbox.h"

namespace geeL {

	class BoundedObject {

	public:
		const AABoundingBox& getBoundingBox() const;

	protected:
		AABoundingBox aabb;

	};

	inline const AABoundingBox& BoundedObject::getBoundingBox() const {
		return aabb;
	}

}

#endif