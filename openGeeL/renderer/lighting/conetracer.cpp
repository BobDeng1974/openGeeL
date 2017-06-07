#include "../voxelization/voxelizer.h"
#include "../voxelization/voxeloctree.h"
#include "../scene.h"
#include "conetracer.h"

namespace geeL {

	VoxelConeTracer::VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, Voxelizer& voxelizer) :
		SceneRender(scene, "renderer/lighting/conetracer.frag"), octree(octree), voxelizer(voxelizer) {}


	void VoxelConeTracer::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		octree.build();

		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void VoxelConeTracer::bindValues() {
		octree.bind(shader);

		shader.setMat4(invViewLocation, camera->getInverseViewMatrix());
		shader.setVector3(originLocation, camera->GetOriginInViewSpace());
	}

	void VoxelConeTracer::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		addBuffer(*maps[WorldMaps::PositionDepth], "gPositionDepth");
		addBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");
	}

}