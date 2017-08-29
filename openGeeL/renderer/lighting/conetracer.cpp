#include "voxelization/voxelizer.h"
#include "voxelization/voxeloctree.h"
#include "voxelization/voxeltexture.h"
#include "voxelization/voxelstructure.h"
#include "renderscene.h"
#include "conetracer.h"

namespace geeL {

	VoxelConeTracer::VoxelConeTracer(RenderScene& scene, VoxelOctree& octree, int maxStep) :
		SceneRender(scene), PostProcessingEffectFS("renderer/lighting/conetracer.frag"), voxelStructure(octree),
			maxStepDiffuse(maxStep), maxStepSpecular(maxStep), specularLOD(1.f), diffuseLOD(1.f) {}

	VoxelConeTracer::VoxelConeTracer(RenderScene& scene, VoxelTexture& texture, int maxStepSpecular, int maxStepDiffuse) :
		SceneRender(scene), PostProcessingEffectFS("renderer/lighting/conetracerTex.frag"), voxelStructure(texture),
			maxStepSpecular(maxStepSpecular), maxStepDiffuse(maxStepDiffuse), specularLOD(1.f), diffuseLOD(1.f) {
	
		texture.bindTexture(shader, "voxelTexture");
	}


	void VoxelConeTracer::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.bind<int>("maxStepSpecular", maxStepSpecular);
		shader.bind<int>("maxStepDiffuse", maxStepDiffuse);
		shader.bind<float>("specularLOD", specularLOD);
		shader.bind<float>("diffuseLOD", diffuseLOD);

		scene.init();
		voxelStructure.build();

		farPlaneLocation = shader.getLocation("farClip");
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void VoxelConeTracer::draw() {
		PostProcessingEffectFS::draw();
	}

	void VoxelConeTracer::bindValues() {
		voxelStructure.bind(shader);

		shader.bind<float>(farPlaneLocation, scene.getCamera().getFarPlane());
		shader.bind<glm::mat4>(invViewLocation, camera->getInverseViewMatrix());
		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
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

			shader.bind<int>("maxStepSpecular", maxStepSpecular);
		}
	}

	void VoxelConeTracer::setDiffuseSampleSize(unsigned int size) {
		if (size != maxStepDiffuse && size < 50) {
			maxStepDiffuse = size;

			shader.bind<int>("maxStepDiffuse", maxStepDiffuse);
		}
	}

	void VoxelConeTracer::setSpecularLOD(unsigned int level) {
		if (level > 0) {
			specularLOD = 0.25f * float(level);

			shader.bind<float>("specularLOD", specularLOD);
		}
	}

	void VoxelConeTracer::setDiffuseLOD(unsigned int level) {
		if (level > 0) {
			diffuseLOD = 0.25f * float(level);

			shader.bind<float>("diffuseLOD", diffuseLOD);
		}
	}

}