#include "../voxelization/voxelizer.h"
#include "../voxelization/voxeloctree.h"
#include "../voxelization/voxeltexture.h"
#include "../voxelization/voxelstructure.h"
#include "../scene.h"
#include "conetracer.h"

namespace geeL {

	VoxelConeTracer::VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, int minStep) :
		SceneRender(scene, "renderer/lighting/conetracer.frag"), voxelStructure(octree), minStep(minStep) {}

	VoxelConeTracer::VoxelConeTracer(RenderScene& scene, VoxelTexture& texture, int minStep) :
		SceneRender(scene, "renderer/lighting/conetracerTex.frag"), voxelStructure(texture), minStep(minStep) {
	
		texture.bindTexture(shader, "voxelTexture");
	}


	void VoxelConeTracer::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setInteger("minStep", minStep);

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

	int VoxelConeTracer::getSampleSize() const {
		return minStep;
	}

	void VoxelConeTracer::setSampleSize(unsigned int size) {
		if (size != minStep && size < 50) {
			minStep = size;

			shader.use();
			shader.setInteger("minStep", minStep);
		}
	}

}