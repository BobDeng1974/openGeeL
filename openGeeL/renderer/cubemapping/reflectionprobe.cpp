#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../framebuffer/framebuffer.h"
#include "../transformation/transform.h"
#include "../shader/shader.h"
#include "../cameras/camera.h"
#include "reflectionprobe.h"

using namespace glm;

namespace geeL {

	ReflectionProbe::ReflectionProbe(std::function<void(const Camera&, FrameBufferInformation)> renderCall, Transform& transform,
		unsigned int resolution, float width, float height, float depth, std::string name)
			: SceneObject(transform, name), width(width), height(height), depth(depth), resolution(resolution), renderCall(renderCall) {

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

		unsigned int rbo;
		glGenFramebuffers(1, &fbo);
		glGenRenderbuffers(1, &rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	ReflectionProbe::~ReflectionProbe() {
		FrameBuffer::remove(fbo);
	}


	void ReflectionProbe::update() {

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

		FrameBufferInformation info;
		info.fbo = fbo;
		info.width = resolution;
		info.height = resolution;

		FrameBuffer::bind(fbo);
		glViewport(0, 0, resolution, resolution);
		for (unsigned int side = 0; side < 6; side++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, id, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cam.setViewMatrix(views[side]);
			cam.setProjectionMatrix(projections[side / 2]);
			renderCall(cam, info);
		}

		FrameBuffer::unbind();

		//Mip map rendered environment map
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}