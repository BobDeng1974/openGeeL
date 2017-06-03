#ifndef VOXELOCTREE_H
#define VOXELOCTREE_H

#include <vector>
#include "../framebuffer/bufferutil.h"

typedef unsigned int uint;

namespace geeL {

	class ComputeShader;
	class Voxelizer;

	//Sparse voxel octree implementation
	class VoxelOctree {

	public:
		VoxelOctree(Voxelizer& voxelizer, unsigned int treeLevels = 7);
		~VoxelOctree();

		void build();

	private:
		unsigned int maxLevel, maxNodes;
		unsigned int nodeCounter; //Atomic buffer for node counting. Stores buffer ID, not the actual size
		std::vector<unsigned int> nodeLevels; //Records number of nodes per level

		ComputeShader* flagShader;
		ComputeShader* allocShader;
		ComputeShader* initShader;
		ComputeShader* fillLeavesShader;

		TextureBuffer nodeIndicies; //Stores indicies of child nodes
		TextureBuffer nodeDiffuse;  //Stores color of child nodes
		Voxelizer& voxelizer;

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

