#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "texturing/rendertexturecube.h"
#include "framebuffer/framebuffer.h"
#include "framebuffer/cubebuffer.h"
#include "transformation/transform.h"
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "cameras/camera.h"
#include "reflectionprobe.h"

using namespace glm;


namespace geeL {

	ReflectionProbe::ReflectionProbe(CubeBuffer& frameBuffer, 
		std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall,
		Transform& transform, 
		unsigned int resolution, 
		float width, 
		float height, 
		float depth, 
		std::string name)
			: DynamicCubeMap(new RenderTextureCube(resolution))
			, SceneObject(transform, name)
			, frameBuffer(frameBuffer)
			, width(width)
			, height(height)
			, depth(depth)
			, renderCall(renderCall) {}


	ReflectionProbe::~ReflectionProbe() {
		//delete texture;
	}


	void ReflectionProbe::bind(const Camera& camera, const RenderShader& shader,
		const std::string& name, ShaderTransformSpace space) const {

		vec3 offset = vec3(width, height, depth);
		vec3 minPos = transform.getPosition() - offset;
		vec3 maxPos = transform.getPosition() + offset;

		switch (space) {
			case ShaderTransformSpace::View:
				/*
				shader.bind<glm::vec3>(name + "centerPosition", camera.TranslateToViewSpace(transform.getPosition()));
				shader.bind<glm::vec3>(name + "minPosition", camera.TranslateToViewSpace(minPos));
				shader.bind<glm::vec3>(name + "maxPosition", camera.TranslateToViewSpace(maxPos));
				break;
				*/
			case ShaderTransformSpace::World:
				shader.bind<glm::vec3>(name + "centerPosition", transform.getPosition());
				shader.bind<glm::vec3>(name + "minPosition", minPos);
				shader.bind<glm::vec3>(name + "maxPosition", maxPos);
				break;
		}

		shader.bind<int>(name + "global", false);
	}


	void ReflectionProbe::draw() {
		frameBuffer.init(*texture);

		ManualCamera& cam = ManualCamera(transform);

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

		texture->mipmap();
	}

}