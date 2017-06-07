#ifndef VOXELCONETRACING_H
#define VOXELCONETRACING_H

#include "scenerender.h"

namespace geeL {

	class Voxelizer;
	class VoxelOctree;

	class VoxelConeTracer : public SceneRender {

	public:
		VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, Voxelizer& voxelizer);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	protected:
		virtual void bindValues();

	private:
		Voxelizer& voxelizer;
		VoxelOctree& octree;

		ShaderLocation invViewLocation;
		ShaderLocation originLocation;


	};
}

#endif