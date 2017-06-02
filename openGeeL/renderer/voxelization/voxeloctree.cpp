#define GLEW_STATIC
#include <glew.h>
#include "../shader/computeshader.h"
#include "voxelizer.h"
#include "voxeloctree.h"

namespace geeL {

	VoxelOctree::VoxelOctree(Voxelizer& voxelizer, unsigned int treeLevels) 
		: voxelizer(voxelizer), maxLevel(treeLevels), nodeCounter(0) {
	
		flagShader = new ComputeShader("renderer/voxelization/nodeFlag.com.glsl");
		allocShader = new ComputeShader("renderer/voxelization/nodeAlloc.com.glsl");
		initShader = new ComputeShader("renderer/voxelization/nodeInit.com.glsl");

		computeMaximum();
		nodeLevels.push_back(1); //Root has always one node
	}

	VoxelOctree::~VoxelOctree() {
		delete flagShader;
		delete allocShader;
		delete initShader;
	}


	void VoxelOctree::buildOctree() {
		BufferUtility::generateTextureBuffer(maxNodes * sizeof(unsigned int), GL_R32UI, nodeIndicies);
		BufferUtility::generateTextureBuffer(maxNodes * sizeof(unsigned int), GL_R32UI, nodeDiffuse);
		BufferUtility::generateAtomicBuffer(nodeCounter);

		for (int i = 0; i < maxLevel; i++) {


		}

	}

	void VoxelOctree::computeMaximum() {
		maxNodes = 1;
		unsigned int currLevel = 1;

		for (int i = 0; i < maxLevel; i++) {
			currLevel *= 8;
			maxNodes += currLevel;
		}
	}

}