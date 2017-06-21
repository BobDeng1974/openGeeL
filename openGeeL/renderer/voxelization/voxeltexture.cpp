#define GLEW_STATIC
#include <glew.h>
#include "../texturing/texture3D.h"
#include "../shader/sceneshader.h"
#include "../scene.h"
#include "voxeltexture.h"

namespace geeL {

	VoxelTexture::VoxelTexture(const RenderScene& scene, unsigned int dimensions) 
		: scene(scene), dimensions(dimensions){

		FragmentShader frag = FragmentShader("renderer/voxelization/voxelizeTex.frag", true, false);
		voxelShader = new SceneShader("renderer/voxelization/voxelize.vert", "renderer/voxelization/voxelize.geom",
			frag, ShaderTransformSpace::World);

		voxelShader->mapOffset = 1;

		texture = new Texture3D(dimensions, dimensions, dimensions, 7);
		bindTexture(*voxelShader, "voxelTexture");
	}

	VoxelTexture::~VoxelTexture() {
		delete texture;
	}


	void VoxelTexture::build() {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, dimensions, dimensions);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);	
		glDisable(GL_BLEND);

		voxelShader->use();
		glBindImageTexture(0, texture->getID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

		//Render scene
		scene.drawObjects(*voxelShader);

		glBindTexture(GL_TEXTURE_3D, texture->getID());
		glGenerateMipmap(GL_TEXTURE_3D);;

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}


	void VoxelTexture::bind(const Shader& shader) const {
		shader.use();
		shader.setInteger("dimensions", dimensions);
	}

	void VoxelTexture::bindTexture(Shader & shader, const std::string& name) {
		shader.addMap(texture->getID(), name, GL_TEXTURE_3D);
	}

}