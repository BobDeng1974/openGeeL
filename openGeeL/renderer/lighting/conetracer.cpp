#include "../voxelization/voxelizer.h"
#include "../voxelization/voxeloctree.h"
#include "../scene.h"
#include "conetracer.h"

namespace geeL {

	VoxelConeTracer::VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, Voxelizer& voxelizer, int minStep) :
		SceneRender(scene, "renderer/lighting/conetracer.frag"), octree(octree), voxelizer(voxelizer), minStep(minStep) {}


	void VoxelConeTracer::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setInteger("minStep", minStep);

		octree.build();

		farPlaneLocation = shader.getLocation("farClip");
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void VoxelConeTracer::bindValues() {
		octree.bind(shader);

		shader.setFloat(farPlaneLocation, scene.getCamera().getFarPlane());
		shader.setMat4(invViewLocation, camera->getInverseViewMatrix());
		shader.setVector3(originLocation, camera->GetOriginInViewSpace());
	}

	void VoxelConeTracer::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		addBuffer(*maps[WorldMaps::PositionDepth], "gPositionDepth");
		addBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");
		addBuffer(*maps[WorldMaps::DiffuseRoughness], "gDiffuseSpec");
	}

}