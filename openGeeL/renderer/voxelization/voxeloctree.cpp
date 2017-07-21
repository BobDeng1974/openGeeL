#define GLEW_STATIC
#include <glew.h>
#include <cmath>
#include <iostream>
#include "../shader/computeshader.h"
#include "voxelizer.h"
#include "voxeloctree.h"

namespace geeL {

	VoxelOctree::VoxelOctree(Voxelizer& voxelizer, unsigned int treeLevels) 
		: voxelizer(voxelizer), nodeCounter(0) {
	
		flagShader = new ComputeShader("renderer/voxelization/nodeFlag.com.glsl");
		allocShader = new ComputeShader("renderer/voxelization/nodeAlloc.com.glsl");
		initShader = new ComputeShader("renderer/voxelization/nodeInit.com.glsl");
		fillLeavesShader = new ComputeShader("renderer/voxelization/leavesFill.com.glsl");
		mipmapShader = new ComputeShader("renderer/voxelization/nodeMipmap.com.glsl");

		//Clamp tree levels
		unsigned int levelMax = log(voxelizer.getDimensions()) / log(2) - 1;
		maxLevel = (treeLevels > levelMax) ? levelMax : treeLevels;

		computeMaximum();
	}

	VoxelOctree::~VoxelOctree() {
		delete flagShader;
		delete allocShader;
		delete initShader;
		delete fillLeavesShader;
	}


	void VoxelOctree::build() {
		voxelizer.voxelize();

		nodeLevels.clear();
		nodeLevels.push_back(1); //Root has always one node

		BufferUtility::generateTextureBuffer(maxNodes * sizeof(unsigned int), GL_R32UI, nodeIndicies);
		BufferUtility::generateTextureBuffer(maxNodes * sizeof(unsigned int), GL_R32UI, nodeDiffuse);
		BufferUtility::generateAtomicBuffer(nodeCounter);

		unsigned int numVoxels  = voxelizer.getVoxelAmount();
		unsigned int dataWidth  = 1024;	//Note: hardcoded in shaders
		unsigned int dataHeight = (numVoxels + (dataWidth - 1)) / dataWidth; //Size will be increased for every row the voxels fill

		//Division by 8 because work groups consist of 8x8 threads
		unsigned int groupWidth  = dataWidth / 8; 
		unsigned int groupHeight = (dataHeight + 7) / 8;

		buildOctree(dataWidth, groupWidth, groupHeight);
		buildLeafNodes(groupWidth, groupHeight);
		mipmapOctree();

		glDeleteBuffers(1, &nodeCounter);
		nodeCounter = 0;
	}

	void VoxelOctree::bind(const Shader& shader) const {
		shader.use();
		shader.bind<int>("level", maxLevel);
		shader.bind<int>("dimensions", voxelizer.getDimensions());

		glBindImageTexture(0, nodeIndicies.texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glBindImageTexture(1, nodeDiffuse.texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
	}


	void VoxelOctree::buildOctree(uint dataWidth, uint groupWidth, uint groupHeight) {
		unsigned int nodeOffset  = 0; //Offset in 1D data structure to nodes in current level
		unsigned int allocOffset = 1; //Offset in 1D data structure to free space (Where nodes of next level will be allocated)
		unsigned int dataHeight;

		for (int i = 0; i < maxLevel; i++) {
			//Determine whether nodes should have children or not
			flagShader->use();
			flagShader->bind<int>("numVoxels", voxelizer.getVoxelAmount());
			flagShader->bind<int>("level", i);
			flagShader->bind<int>("dimensions", voxelizer.getDimensions());
			glBindImageTexture(0, voxelizer.getVoxelPositions(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI);
			glBindImageTexture(1, nodeIndicies.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

			flagShader->invoke(groupWidth, groupHeight, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			
			//Allocate tiles for new children (1 tile = 8 children nodes)
			unsigned int currNodes = nodeLevels[i]; //Node amount in current level
			allocShader->use();
			allocShader->bind<int>("numNodes", currNodes);
			allocShader->bind<int>("nodeOffset", nodeOffset);
			allocShader->bind<int>("allocOffset", allocOffset);
			glBindImageTexture(0, nodeIndicies.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, nodeCounter);

			unsigned int allocGroupWidth = (currNodes + 63) / 64;
			allocShader->invoke(allocGroupWidth, 1, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);


			//Get number of tiles from atomic node counter;
			unsigned int allocatedTiles;
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, nodeCounter);
			glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &allocatedTiles);

			//Reset node counter
			unsigned int reset = 0;
			glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &reset);
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);


			//Initialize new node tiles
			unsigned int allocatedNodes = 8 * allocatedTiles;
			initShader->use();
			initShader->bind<int>("numNodes", allocatedNodes);
			initShader->bind<int>("allocOffset", allocOffset);

			glBindImageTexture(0, nodeIndicies.texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);
			glBindImageTexture(1, nodeDiffuse.texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);

			dataHeight = (allocatedNodes + (dataWidth - 1)) / dataWidth;
			unsigned int initGroupHeight = (dataHeight + 7) / 8;
			initShader->invoke(groupWidth, initGroupHeight, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			//Update offsets
			nodeLevels.push_back(allocatedNodes);
			nodeOffset += nodeLevels[i];
			allocOffset += allocatedNodes;
		}
	}

	void VoxelOctree::buildLeafNodes(uint width, uint height) {
		
		//Write voxel information into leaves
		fillLeavesShader->use();
		fillLeavesShader->bind<int>("numVoxels", voxelizer.getVoxelAmount());
		fillLeavesShader->bind<int>("level", maxLevel);
		fillLeavesShader->bind<int>("dimensions", voxelizer.getDimensions());

		glBindImageTexture(0, nodeIndicies.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, nodeDiffuse.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(2, voxelizer.getVoxelPositions(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI);
		glBindImageTexture(3, voxelizer.getVoxelColors(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);

		fillLeavesShader->invoke(width, height, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}


	void VoxelOctree::mipmapOctree() {
		unsigned int nodeOffset = 0;
		for (int i = 0; i < nodeLevels.size() - 1; i++)
			nodeOffset += nodeLevels[i];

		for (int i = maxLevel - 1; i > 0; i--) {
			nodeOffset -= nodeLevels[i];

			unsigned int currNodes = nodeLevels[i];
			mipmapShader->use();
			mipmapShader->bind<int>("numNodes", currNodes);
			mipmapShader->bind<int>("nodeOffset", nodeOffset);

			glBindImageTexture(0, nodeIndicies.texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
			glBindImageTexture(1, nodeDiffuse.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

			unsigned int allocGroupWidth = (currNodes + 63) / 64;
			allocShader->invoke(allocGroupWidth, 1, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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