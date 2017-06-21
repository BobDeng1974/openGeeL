#ifndef VOXELSTRUCTURE_H
#define VOXELSTRUCTURE_H

namespace geeL {

	class Shader;

	//Base class for all structures that store scene information in voxels
	class VoxelStructure {

	public:
		virtual void build() = 0;
		virtual void bind(const Shader& shader) const = 0;

	};

}

#endif

