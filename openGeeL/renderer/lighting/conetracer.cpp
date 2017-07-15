#include "../voxelization/voxelizer.h"
#include "../voxelization/voxeloctree.h"
#include "../voxelization/voxeltexture.h"
#include "../voxelization/voxelstructure.h"
#include "../scene.h"
#include "conetracer.h"

namespace geeL {

	VoxelConeTracer::VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, int maxStep) :
		SceneRender(scene, "renderer/lighting/conetracer.frag"), voxelStructure(octree), 
			maxStepDiffuse(maxStep), maxStepSpecular(maxStep), specularLOD(1.f), diffuseLOD(1.f) {}

	VoxelConeTracer::VoxelConeTracer(RenderScene& scene, VoxelTexture& texture, int maxStepSpecular, int maxStepDiffuse) :
		SceneRender(scene, "renderer/lighting/conetracerTex.frag"), voxelStructure(texture), 
			maxStepSpecular(maxStepSpecular), maxStepDiffuse(maxStepDiffuse), specularLOD(1.f), diffuseLOD(1.f) {
	
		texture.bindTexture(shader, "voxelTexture");
	}


	void VoxelConeTracer::init(ScreenQuad& screen, IFrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setInteger("maxStepSpecular", maxStepSpecular);
		shader.setInteger("maxStepDiffuse", maxStepDiffuse);
		shader.setFloat("specularLOD", specularLOD);
		shader.setFloat("diffuseLOD", diffuseLOD);

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
		addImageBuffer(*maps[WorldMaps::PositionRoughness], "gPositionRoughness");
		addImageBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");
		addImageBuffer(*maps[WorldMaps::Diffuse], "gDiffuse");
	}

	unsigned int VoxelConeTracer::getSpecularSampleSize() const {
		return maxStepSpecular;
	}

	unsigned int VoxelConeTracer::getDiffuseSampleSize() const {
		return maxStepDiffuse;
	}

	unsigned int VoxelConeTracer::getSpecularLOD() const {
		return unsigned int(4.f * specularLOD);
	}

	unsigned int VoxelConeTracer::getDiffuseLOD() const {
		return unsigned int(4.f * diffuseLOD);
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

	void VoxelConeTracer::setSpecularLOD(unsigned int level) {
		if (level > 0) {
			specularLOD = 0.25f * float(level);

			shader.use();
			shader.setFloat("specularLOD", specularLOD);
		}
	}

	void VoxelConeTracer::setDiffuseLOD(unsigned int level) {
		if (level > 0) {
			diffuseLOD = 0.25f * float(level);

			shader.use();
			shader.setFloat("diffuseLOD", diffuseLOD);
		}
	}

}