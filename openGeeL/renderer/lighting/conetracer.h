#ifndef VOXELCONETRACING_H
#define VOXELCONETRACING_H

#include "scenerender.h"

namespace geeL {

	class SceneCamera;
	class Voxelizer;
	class VoxelOctree;

	class VoxelConeTracer : public SceneRender {

	public:
		VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, Voxelizer& voxelizer, int minStep = 15);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	protected:
		virtual void bindValues();

	private:
		int minStep;
		SceneCamera* sceneCamera;
		Voxelizer& voxelizer;
		VoxelOctree& octree;

		ShaderLocation farPlaneLocation;
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};
}

#endif