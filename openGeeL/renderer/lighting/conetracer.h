#ifndef VOXELCONETRACING_H
#define VOXELCONETRACING_H

#include "scenerender.h"

namespace geeL {

	class SceneCamera;
	class Voxelizer;
	class VoxelOctree;
	class VoxelStructure;
	class VoxelTexture;

	class VoxelConeTracer : public SceneRender, public PostProcessingEffectFS {

	public:
		VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, int maxStep = 5);
		VoxelConeTracer(RenderScene& scene, VoxelTexture& texture, int maxStepSpecular = 20, int maxStepDiffuse = 10);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();
		virtual void draw();
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		unsigned int getSpecularSampleSize() const;
		unsigned int getDiffuseSampleSize() const;
		unsigned int getSpecularLOD() const;
		unsigned int getDiffuseLOD() const;

		void setSpecularSampleSize(unsigned int size);
		void setDiffuseSampleSize(unsigned int size);
		void setSpecularLOD(unsigned int level);
		void setDiffuseLOD(unsigned int level);

		virtual std::string toString() const;

	private:
		unsigned int maxStepSpecular, maxStepDiffuse;
		float specularLOD, diffuseLOD;
		SceneCamera* sceneCamera;
		VoxelStructure& voxelStructure;

		ShaderLocation farPlaneLocation;
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};


	inline std::string VoxelConeTracer::toString() const {
		return "Voxel Cone Tracder";
	}

}

#endif