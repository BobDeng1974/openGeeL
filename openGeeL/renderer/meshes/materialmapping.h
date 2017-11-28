#ifndef MATERIALMAPPING_H
#define MATERIALMAPPING_H

#include "materials/material.h"
#include "glwrapper/masking.h"

namespace geeL {

	class MeshInstance;

	struct MaterialMapping {
		const MeshInstance& mesh;
		Material material;
		RenderMask mask;

		MaterialMapping(const MeshInstance& mesh, Material material)
			: mesh(mesh)
			, material(std::move(material))
			, mask(RenderMask::None) {}

		MaterialMapping(const MeshInstance& mesh, Material material, RenderMask mask)
			: mesh(mesh)
			, material(std::move(material))
			, mask(mask) {}

		bool operator== (const MaterialMapping& rhs) {
			return &mesh == &rhs.mesh;
		}

	};

}

#endif
