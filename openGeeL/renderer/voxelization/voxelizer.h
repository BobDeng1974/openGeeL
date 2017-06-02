#ifndef VOXELIZER_H
#define VOXELIZER_H

#include "../framebuffer/bufferutil.h"

namespace geeL {

	class RenderScene;
	class SceneShader;

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

	private:
		unsigned int dimensions, atomicBuffer, voxelAmount;
		TextureBuffer voxelPosition, voxelNormal, voxelColor;
		SceneShader* voxelShader;
		const RenderScene& scene;

		void voxelizeScene(bool drawVoxel) const;

	};
}

#endif