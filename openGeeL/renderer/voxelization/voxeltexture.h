#ifndef VOXELTEXTURE_H
#define VOXELTEXTURE_H

#include <string>
#include "voxelstructure.h"

namespace geeL {

	class RenderScene;
	class SceneShader;
	class Shader;
	class Texture3D;


	class VoxelTexture : public VoxelStructure {

	public:
		VoxelTexture(const RenderScene& scene, unsigned int dimensions = 256);
		~VoxelTexture();

		virtual void build();
		virtual void bind(const Shader& shader) const;

		//Bind underlying 3D voxel texture into given shader
		void bindTexture(Shader& shader, const std::string& name);

	private:
		unsigned int dimensions;
		const RenderScene& scene;
		SceneShader* voxelShader;
		Texture3D* texture;

	};

}

#endif
