#ifndef VOXELOCTREE_H
#define VOXELOCTREE_H

#include <vector>
#include "framebuffer/bufferutil.h"
#include "voxelstructure.h"

typedef unsigned int uint;

namespace geeL {

	class ComputeShader;
	class Shader;
	class Voxelizer;

	//Sparse voxel octree implementation
	class VoxelOctree : public VoxelStructure {

	public:
		VoxelOctree(Voxelizer& voxelizer, unsigned int treeLevels = 7);
		~VoxelOctree();

		virtual void build();
		virtual void bind(const Shader& shader) const;

	private:
		unsigned int maxLevel, maxNodes;
		unsigned int nodeCounter; //Atomic buffer for node counting. Stores buffer ID, not the actual size
		std::vector<unsigned int> nodeLevels; //Records number of nodes per level

		ComputeShader* flagShader;
		ComputeShader* allocShader;
		ComputeShader* initShader;
		ComputeShader* fillLeavesShader;
		ComputeShader* mipmapShader;

		TextureBuffer nodeIndicies; //Stores indicies of child nodes
		TextureBuffer nodeDiffuse;  //Stores color of child nodes
		Voxelizer& voxelizer;

		VoxelOctree(const VoxelOctree& other) = delete;
		VoxelOctree& operator= (const VoxelOctree& other) = delete;

		//Sort voxels into tree nodes
		void buildOctree(uint dataWidth, uint groupWidth, uint groupHeight);

		//Stores surface information in leaf nodes
		void buildLeafNodes(uint width, uint height);

		//Mipmap voxel information in leaves into whole tree structure
		void mipmapOctree();

		//Computes the maximum possible node amount
		void computeMaximum();

	};
}

#endif

