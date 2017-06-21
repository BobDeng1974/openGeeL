#ifndef VOXELCONETRACING_H
#define VOXELCONETRACING_H

#include "scenerender.h"

namespace geeL {

	class SceneCamera;
	class Voxelizer;
	class VoxelOctree;
	class VoxelStructure;
	class VoxelTexture;

	class VoxelConeTracer : public SceneRender {

	public:
		VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, int minStep = 5);
		VoxelConeTracer(RenderScene& scene, VoxelTexture& texture, int minStep = 8);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	protected:
		virtual void bindValues();

	private:
		int minStep;
		SceneCamera* sceneCamera;
		VoxelStructure& voxelStructure;

		ShaderLocation farPlaneLocation;
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};
}

#endif