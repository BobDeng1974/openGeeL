#ifndef VOXELIZER_H
#define VOXELIZER_H

#include "../framebuffer/bufferutil.h"

namespace geeL {

	class RenderScene;
	class SceneShader;

	//Voxelized scene and stores voxels into a buffer list
	class Voxelizer {

	public:
		Voxelizer(const RenderScene& scene, unsigned int dimensions = 256);
		~Voxelizer();

		//Build scene into a voxel list
		void voxelize();

		unsigned int getVoxelAmount() const;
		unsigned int getDimensions() const;

		//Returns ID of data structure that stores voxel positions
		unsigned int getVoxelPositions() const;

		//Returns ID of data structure that stores voxel colors
		unsigned int getVoxelColors() const;

		//Returns ID of data structure that stores voxel normals
		unsigned int getVoxelNormals() const;

	private:
		unsigned int dimensions, atomicBuffer, voxelAmount;
		TextureBuffer voxelPositions, voxelNormals, voxelColors;
		SceneShader* voxelShader;
		const RenderScene& scene;

		void voxelizeScene(bool drawVoxel) const;

	};
}

#endif