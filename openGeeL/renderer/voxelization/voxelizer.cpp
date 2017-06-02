#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include "../shader/sceneshader.h"
#include "../scene.h"
#include "../lights/lightmanager.h"
#include "voxelizer.h"

namespace geeL {

	Voxelizer::Voxelizer(const RenderScene& scene, unsigned int dimensions) : scene(scene), dimensions(dimensions) {
		FragmentShader frag = FragmentShader("renderer/voxelization/voxelize.frag");
		voxelShader = new SceneShader("renderer/voxelization/voxelize.vert", "renderer/voxelization/voxelize.geom", 
			frag, ShaderTransformSpace::World);

	}

	Voxelizer::~Voxelizer() {
		delete voxelShader;
	}


	void Voxelizer::voxelize() {
		BufferUtility::generateAtomicBuffer(atomicBuffer);

		//Pass 1: Get number of voxels and generate buffers accordingly
		voxelizeScene(false);
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicBuffer);
		unsigned int* count = (unsigned int*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, 
			sizeof(unsigned int), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

		voxelAmount = count[0];
		BufferUtility::generateTextureBuffer(voxelAmount * sizeof(unsigned int), GL_R32UI, voxelPosition);
		BufferUtility::generateTextureBuffer(voxelAmount * sizeof(unsigned int), GL_RGBA8, voxelColor);
		BufferUtility::generateTextureBuffer(voxelAmount * sizeof(unsigned int), GL_RGBA16F, voxelNormal);

		//Reset counter
		memset(count, 0, sizeof(unsigned int));
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);


		//Pass 2: Draw and store voxels in previously created buffers 
		voxelizeScene(true);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	void Voxelizer::voxelizeScene(bool drawVoxel) const {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, dimensions, dimensions);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		//Bind to location 0
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicBuffer);

		voxelShader->use();
		voxelShader->setVector2("resolution", glm::vec2(dimensions));
		voxelShader->setInteger("drawVoxel", (int)drawVoxel);

		if (drawVoxel) {
			glBindImageTexture(0, voxelPosition.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI);
			glBindImageTexture(1, voxelColor.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
			glBindImageTexture(2, voxelNormal.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
			voxelShader->setInteger("voxelPosition", 0);
		}

		//Render scene
		scene.lightManager.bindShadowMaps(*voxelShader);
		scene.drawObjects(*voxelShader);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}


	unsigned int Voxelizer::getVoxelAmount() const {
		return voxelAmount;
	}

	unsigned int Voxelizer::getDimensions() const {
		return dimensions;
	}

	unsigned int Voxelizer::getVoxelPositions() const {
		return voxelPosition.texture;
	}


}