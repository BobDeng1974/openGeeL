#ifndef MATERIALMAPPING_H
#define MATERIALMAPPING_H

#include "materials/material.h"
#include "utility/masking.h"

namespace geeL {

	class Mesh;

	struct MaterialMapping {
		Material material;
		RenderMask mask;

		MaterialMapping(Material material)
			: material(std::move(material))
			, mask(RenderMask::None) {}

		MaterialMapping(Material material, RenderMask mask)
			: material(std::move(material))
			, mask(mask) {}

		virtual const Mesh& getMesh() const = 0;

		bool operator== (const MaterialMapping& rhs) {
			return &getMesh() == &rhs.getMesh();
		}

	};

	template<typename MeshType>
	struct GenericMaterialMapping : public MaterialMapping {
		const MeshType& mesh;

		template<typename ...MappingArgs>
		GenericMaterialMapping(const MeshType& mesh, MappingArgs&& ...args);

		virtual const Mesh& getMesh() const;

	};


	template<typename MeshType>
	template<typename ...MappingArgs>
	inline GenericMaterialMapping<MeshType>::GenericMaterialMapping(const MeshType& mesh, MappingArgs&& ...args)
		: MaterialMapping(std::forward<MappingArgs>(args)...)
		, mesh(mesh) {}

	template<typename MeshType>
	inline const Mesh& GenericMaterialMapping<MeshType>::getMesh() const {
		return mesh;
	}

}

#endif
