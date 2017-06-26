#include "../voxelization/voxelizer.h"
#include "../voxelization/voxeloctree.h"
#include "../voxelization/voxeltexture.h"
#include "../voxelization/voxelstructure.h"
#include "../scene.h"
#include "conetracer.h"

namespace geeL {

	VoxelConeTracer::VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, int maxStep) :
		SceneRender(scene, "renderer/lighting/conetracer.frag"), 
			voxelStructure(octree), maxStepDiffuse(maxStep), maxStepSpecular(maxStep) {}

	VoxelConeTracer::VoxelConeTracer(RenderScene& scene, VoxelTexture& texture, int maxStepSpecular, int maxStepDiffuse) :
		SceneRender(scene, "renderer/lighting/conetracerTex.frag"), 
			voxelStructure(texture), maxStepSpecular(maxStepSpecular), maxStepDiffuse(maxStepDiffuse) {
	
		texture.bindTexture(shader, "voxelTexture");
	}


	void VoxelConeTracer::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setInteger("maxStepSpecular", maxStepSpecular);
		shader.setInteger("maxStepDiffuse", maxStepDiffuse);

		voxelStructure.build();

		farPlaneLocation = shader.getLocation("farClip");
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void VoxelConeTracer::bindValues() {
		voxelStructure.bind(shader);

		shader.setFloat(farPlaneLocation, scene.getCamera().getFarPlane());
		shader.setMat4(invViewLocation, camera->getInverseViewMatrix());
		shader.setVector3(originLocation, camera->GetOriginInViewSpace());
	}

	void VoxelConeTracer::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		addBuffer(*maps[WorldMaps::PositionRoughness], "gPositionRoughness");
		addBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");
		addBuffer(*maps[WorldMaps::Diffuse], "gDiffuse");
	}

	int VoxelConeTracer::getSpecularSampleSize() const {
		return maxStepSpecular;
	}

	int VoxelConeTracer::getDiffuseSampleSize() const {
		return maxStepDiffuse;
	}

	void VoxelConeTracer::setSpecularSampleSize(unsigned int size) {
		if (size != maxStepSpecular && size < 50) {
			maxStepSpecular = size;

			shader.use();
			shader.setInteger("maxStepSpecular", maxStepSpecular);
		}
	}

	void VoxelConeTracer::setDiffuseSampleSize(unsigned int size) {
		if (size != maxStepDiffuse && size < 50) {
			maxStepDiffuse = size;

			shader.use();
			shader.setInteger("maxStepDiffuse", maxStepDiffuse);
		}
	}

}