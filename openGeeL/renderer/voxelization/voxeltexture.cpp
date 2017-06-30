#define GLEW_STATIC
#include <glew.h>
#include "../texturing/rendertexture3D.h"
#include "../lights/lightmanager.h"
#include "../transformation/transform.h"
#include "../cameras/camera.h"
#include "../shader/sceneshader.h"
#include "../scene.h"
#include "voxeltexture.h"

namespace geeL {

	VoxelTexture::VoxelTexture(const RenderScene& scene, unsigned int dimensions) 
		: scene(scene), dimensions(dimensions), texture(new RenderTexture3D(dimensions, dimensions, dimensions, 7)) {

		FragmentShader frag = FragmentShader("renderer/voxelization/voxelizeTex.frag", true, false);
		voxelShader = new SceneShader("renderer/voxelization/voxelize.vert", "renderer/voxelization/voxelize.geom",
			frag, ShaderTransformSpace::World);

		voxelShader->mapOffset = 1;
	}

	VoxelTexture::~VoxelTexture() {
		texture->remove();
		delete texture;
	}


	void VoxelTexture::build() {
		texture->clear();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, dimensions, dimensions);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);	

		voxelShader->use();
		voxelShader->setVector2("resolution", glm::vec2(dimensions));

		const Camera& camera = scene.getCamera();
		voxelShader->setVector3("cameraPosition", camera.transform.getPosition());

		scene.lightManager.bindShadowMaps(*voxelShader);

		glBindImageTexture(0, texture->getID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

		//Render scene
		scene.drawObjects(*voxelShader);

		texture->mipmap();

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}


	void VoxelTexture::bind(const Shader& shader) const {
		shader.use();
		shader.setInteger("dimensions", dimensions);
	}

	void VoxelTexture::bindTexture(Shader & shader, const std::string& name) {
		shader.addMap(*texture, name);
	}

	void VoxelTexture::clearTexture() {
		texture->bind();
		Texture::clear(ColorType::RGBA, texture->getID());
	}

}