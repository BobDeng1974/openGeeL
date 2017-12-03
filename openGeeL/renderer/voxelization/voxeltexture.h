#ifndef VOXELTEXTURE_H
#define VOXELTEXTURE_H

#include <string>
#include "texturing/functionaltexture.h"
#include "voxelstructure.h"

namespace geeL {

	class RenderScene;
	class SceneShader;
	class Shader;


	class VoxelTexture : public VoxelStructure, public FunctionalTexture {

	public:
		VoxelTexture(const RenderScene& scene, unsigned int dimensions = 256);

		virtual void build();
		virtual void bind(const Shader& shader) const;

		void clearTexture();

	private:
		unsigned int dimensions;
		const RenderScene& scene;
		SceneShader* voxelShader;

		VoxelTexture(const VoxelTexture& other) = delete;
		VoxelTexture& operator= (const VoxelTexture& other) = delete;

	};

}

#endif
