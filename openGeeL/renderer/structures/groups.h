#ifndef GROUPS_H
#define GROUPS_H

#include "bvh.h"
#include "meshnode.h"
#include "simplegroup.h"

namespace geeL {

	using MeshGroup = SimpleGroup<MeshNode>;
	using MeshTree  = BVHBranch;

}

#endif
