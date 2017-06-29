#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../framebuffer/framebuffer.h"
#include "../framebuffer/cubebuffer.h"
#include "../transformation/transform.h"
#include "../shader/rendershader.h"
#include "../shader/sceneshader.h"
#include "../cameras/camera.h"
#include "reflectionprobe.h"

using namespace glm;

namespace geeL {

	ReflectionProbe::ReflectionProbe(CubeBuffer& frameBuffer, std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall,
		Transform& transform, unsigned int resolution, float width, float height, float depth, std::string name)
			: SceneObject(transform, name), frameBuffer(frameBuffer), width(width), height(height), depth(depth), 
				resolution(resolution), renderCall(renderCall) {

		//Init cubemap without textures
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		for (unsigned int side = 0; side < 6; side++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, GL_RGB16F,
				resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}


	void ReflectionProbe::bind(const Camera& camera, const RenderShader& shader,
		const std::string& name, ShaderTransformSpace space) const {

		vec3 offset = vec3(width, height, depth);
		vec3 minPos = transform.getPosition() - offset;
		vec3 maxPos = transform.getPosition() + offset;

		switch (space) {
			case ShaderTransformSpace::View:
				shader.setVector3(name + "minPosition", camera.TranslateToViewSpace(minPos));
				shader.setVector3(name + "maxPosition", camera.TranslateToViewSpace(maxPos));
				break;
			case ShaderTransformSpace::World:
				shader.setVector3(name + "minPosition", minPos);
				shader.setVector3(name + "maxPosition", maxPos);
				break;
		}
	}

	void ReflectionProbe::update() {
		frameBuffer.init(resolution, id);

		SimpleCamera cam = SimpleCamera(transform);

		glm::mat4 projections[] = {
			perspective(90.0f, 1.0f, 0.1f, width),
			perspective(90.0f, 1.0f, 0.1f, height),
			perspective(90.0f, 1.0f, 0.1f, depth)
		};

		glm::mat4 views[] = {
			lookAt(transform.getPosition(), transform.getPosition() + vec3(1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f)),
			lookAt(transform.getPosition(), transform.getPosition() + vec3(-1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f)),
			lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f)),
			lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, -1.f)),
			lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 0.f, 1.f), vec3(0.f, -1.f, 0.f)),
			lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 0.f, -1.f), vec3(0.f, -1.f, 0.f))
		};

		frameBuffer.fill([&](unsigned int side) {
			cam.setViewMatrix(views[side]);
			cam.setProjectionMatrix(projections[side / 2]);
			renderCall(cam, frameBuffer);
		});

		//Mip map rendered environment map
		Texture::mipmap(TextureType::TextureCube, id);
	}

}