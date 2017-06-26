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
		VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, int maxStep = 5);
		VoxelConeTracer(RenderScene& scene, VoxelTexture& texture, int maxStepSpecular = 15, int maxStepDiffuse = 10);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		int getSpecularSampleSize() const;
		int getDiffuseSampleSize() const;

		void setSpecularSampleSize(unsigned int size);
		void setDiffuseSampleSize(unsigned int size);

	protected:
		virtual void bindValues();

	private:
		int maxStepSpecular, maxStepDiffuse;
		SceneCamera* sceneCamera;
		VoxelStructure& voxelStructure;

		ShaderLocation farPlaneLocation;
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};
}

#endif