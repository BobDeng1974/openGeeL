#ifndef VOXELTEXTURE_H
#define VOXELTEXTURE_H

#include <string>
#include "voxelstructure.h"

namespace geeL {

	class RenderScene;
	class SceneShader;
	class Shader;
	class RenderTexture3D;


	class VoxelTexture : public VoxelStructure {

	public:
		VoxelTexture(const RenderScene& scene, unsigned int dimensions = 256);
		virtual ~VoxelTexture();

		virtual void build();
		virtual void bind(const Shader& shader) const;

		//Bind underlying 3D voxel texture into given shader
		void bindTexture(Shader& shader, const std::string& name);
		void clearTexture();

	private:
		unsigned int dimensions;
		const RenderScene& scene;
		SceneShader* voxelShader;
		RenderTexture3D* texture;

		VoxelTexture(const VoxelTexture& other) = delete;
		VoxelTexture& operator= (const VoxelTexture& other) = delete;

	};

}

#endif
