#ifndef VOXELOCTREE_H
#define VOXELOCTREE_H

#include <vector>
#include "../framebuffer/bufferutil.h"

namespace geeL {

	class ComputeShader;
	class Voxelizer;

	//Sparse voxel octree implementation
	class VoxelOctree {

	public:
		VoxelOctree(Voxelizer& voxelizer, unsigned int treeLevels = 7);
		~VoxelOctree();

		void buildOctree();

	private:
		unsigned int maxLevel, maxNodes;
		unsigned int nodeCounter; //Atomic buffer for node counting. Stores buffer ID, not the actual size
		std::vector<unsigned int> nodeLevels; //Records number of nodes per level

		ComputeShader* flagShader;
		ComputeShader* allocShader;
		ComputeShader* initShader;

		TextureBuffer nodeIndicies; //Stores indicies of child nodes
		TextureBuffer nodeDiffuse;  //Stores color of child nodes
		Voxelizer& voxelizer;


		//Computes the maximum possible node amount
		void computeMaximum();

	};
}

#endif

