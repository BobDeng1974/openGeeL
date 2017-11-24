#ifndef MATERIALMAPPING_H
#define MATERIALMAPPING_H

#include "materials/material.h"
#include "utility/masking.h"

namespace geeL {

	class InstancedMesh;

	struct MaterialMapping {
		const InstancedMesh& mesh;
		Material material;
		RenderMask mask;

		MaterialMapping(const InstancedMesh& mesh, Material material)
			: mesh(mesh)
			, material(std::move(material))
			, mask(RenderMask::None) {}

		MaterialMapping(const InstancedMesh& mesh, Material material, RenderMask mask)
			: mesh(mesh)
			, material(std::move(material))
			, mask(mask) {}

		bool operator== (const MaterialMapping& rhs) {
			return &mesh == &rhs.mesh;
		}

	};

}

#endif
