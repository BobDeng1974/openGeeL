#define GLEW_STATIC
#include <glew.h>
#include "texturing/texture.h"
#include "lights/lightmanager.h"
#include "transformation/transform.h"
#include "cameras/camera.h"
#include "shader/sceneshader.h"
#include "glwrapper/viewport.h"
#include "glwrapper/glguards.h"
#include "renderscene.h"
#include "voxeltexture.h"

namespace geeL {

	VoxelTexture::VoxelTexture(const RenderScene& scene, unsigned int dimensions) 
		: scene(scene), dimensions(dimensions)
		, texture(new Texture3D(dimensions, dimensions, dimensions, 7, 
			ColorType::RGBA8, 
			FilterMode::Trilinear, 
			WrapMode::ClampBorder)) {

		texture->mipmap();

		FragmentShader frag = FragmentShader("shaders/voxelization/voxelizeTex.frag", true, false);
		voxelShader = new SceneShader("shaders/voxelization/voxelize.vert", "shaders/voxelization/voxelize.geom",
			frag, ShaderTransformSpace::World, ShadingMethod::Other);

		voxelShader->mapOffset = 1;
	}

	VoxelTexture::~VoxelTexture() {
		delete texture;
	}


	void VoxelTexture::build() {
		texture->clear();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Viewport::set(0, 0, dimensions, dimensions);
		DepthGuard depthGuard(true);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);

		voxelShader->bind<glm::vec2>("resolution", glm::vec2(float(dimensions)));

		const Camera& camera = scene.getCamera();
		voxelShader->bind<glm::vec3>("cameraPosition", camera.transform.getPosition());

		scene.getLightmanager().bindShadowmaps(*voxelShader);

		texture->bindImage(0, AccessType::Write);

		//Render scene
		scene.drawObjects(*voxelShader);

		texture->mipmap();

		glEnable(GL_CULL_FACE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}


	void VoxelTexture::bind(const Shader& shader) const {
		shader.bind<int>("dimensions", dimensions);
	}

	void VoxelTexture::bindTexture(Shader& shader, const std::string& name) {
		shader.addMap(*texture, name);
	}

	void VoxelTexture::clearTexture() {
		(*texture)->clear();
	}

}