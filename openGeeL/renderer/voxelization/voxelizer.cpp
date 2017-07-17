#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "../shader/sceneshader.h"
#include "../cameras/camera.h"
#include "../transformation/transform.h"
#include "../scene.h"
#include "../lights/lightmanager.h"
#include "../utility/viewport.h"
#include "voxelizer.h"

using namespace glm;

namespace geeL {

	Voxelizer::Voxelizer(const RenderScene& scene, unsigned int dimensions) : scene(scene), dimensions(dimensions) {
		FragmentShader frag = FragmentShader("renderer/voxelization/voxelize.frag", true, false);
		voxelShader = new SceneShader("renderer/voxelization/voxelize.vert", "renderer/voxelization/voxelize.geom", 
			frag, ShaderTransformSpace::World);

		BufferUtility::generateAtomicBuffer(atomicBuffer);
		voxelShader->mapOffset = 1;
	}

	Voxelizer::~Voxelizer() {
		delete voxelShader;

		glDeleteBuffers(1, &atomicBuffer);
	}


	void Voxelizer::voxelize() {
		initVoxelShader();

		//Pass 1: Get number of voxels and generate buffers accordingly
		voxelizeScene(false);
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

		unsigned int* count = BufferUtility::getAtomicBufferCount(atomicBuffer);

		voxelAmount = count[0];
		BufferUtility::generateTextureBuffer(voxelAmount * sizeof(unsigned int), GL_R32UI, voxelPositions);
		BufferUtility::generateTextureBuffer(voxelAmount * sizeof(unsigned int), GL_RGBA8, voxelColors);
		BufferUtility::generateTextureBuffer(voxelAmount * sizeof(unsigned int), GL_RGBA16F, voxelNormals);

		BufferUtility::resetAtomicBuffer(atomicBuffer, count);

		//Pass 2: Draw and store voxels in previously created buffers 
		voxelizeScene(true);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

		BufferUtility::resetAtomicBuffer(atomicBuffer);
	}

	void Voxelizer::initVoxelShader() const {

		//Create transform matrices for X-, Y- and Z-axis
		float scale = 1.f;
		mat4 proj = glm::ortho(-scale, scale, -scale, scale, -scale, scale);
		proj = glm::ortho(-1.f, 1.f, -1.f, 1.f, 1.f, 3.f);
		mat4 transX = proj * glm::lookAt(vec3(2.f, 0.f, 0.f), vec3(0.f), vec3(0.f, 1.f, 0.f));
		mat4 transY = proj * glm::lookAt(vec3(0.f, 2.f, 0.f), vec3(0.f), vec3(0.f, 0.f, -1.f));
		mat4 transZ = proj * glm::lookAt(vec3(0.f, 0.f, 2.f), vec3(0.f), vec3(0.f, 1.f, 0.f));

		voxelShader->use();
		voxelShader->setMat4("transformX", transX);
		voxelShader->setMat4("transformY", transY);
		voxelShader->setMat4("transformZ", transZ);
		voxelShader->setVector2("resolution", glm::vec2(dimensions));

		const Camera& camera = scene.getCamera();
		voxelShader->setVector3("cameraPosition", camera.transform.getPosition());

		scene.lightManager.bindShadowMaps(*voxelShader);
	}

	void Voxelizer::voxelizeScene(bool drawVoxel) const {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Viewport::set(0, 0, dimensions, dimensions);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		voxelShader->use();
		voxelShader->setInteger("drawVoxel", (int)drawVoxel);

		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicBuffer);

		if (drawVoxel) {
			glBindImageTexture(0, voxelPositions.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI);
			glBindImageTexture(1, voxelColors.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
			glBindImageTexture(2, voxelNormals.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
		}

		//Render scene
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
		return voxelPositions.texture;
	}

	unsigned int Voxelizer::getVoxelColors() const {
		return voxelColors.texture;
	}

	unsigned int Voxelizer::getVoxelNormals() const {
		return voxelNormals.texture;
	}


}