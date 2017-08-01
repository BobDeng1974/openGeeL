#ifndef GEELCONFIGURATION_H
#define GEELCONFIGURATION_H

#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <functional>
#include "framebuffer/gbuffer.h"
#include "shader/rendershader.h"
#include "renderer/rendercontext.h"
#include "application.h"

#include "inputmanager.h"
#include "window.h"
#include "texturing/texture.h"
#include "texturing/imagetexture.h"
#include "texturing/layeredtexture.h"

#include "cameras/camera.h"
#include "cameras/perspectivecamera.h"
#include "lights/light.h"
#include "lights/lightmanager.h"
#include "lights/pointlight.h"
#include "lights/directionallight.h"
#include "lights/spotlight.h"
#include "renderer/deferredrenderer.h"

#include "transformation/transform.h"
#include "transformation/transformfactory.h"

#include "materials/material.h"
#include "materials/defaultmaterial.h"
#include "materials/genericmaterial.h"
#include "materials/materialfactory.h"
#include "pipeline.h"
#include "meshes/mesh.h"
#include "meshes/model.h"
#include "meshes/meshrenderer.h"
#include "meshes/skinnedrenderer.h"
#include "meshes/meshfactory.h"

#include "lighting/deferredlighting.h"
#include "lighting/ibl.h"
#include "postprocessing/postprocessing.h"
#include "postprocessing/colorcorrection.h"
#include "postprocessing/gammacorrection.h"
#include "postprocessing/tonemapping.h"
#include "postprocessing/gaussianblur.h"
#include "postprocessing/bloom.h"
#include "postprocessing/godray.h"
#include "postprocessing/ssao.h"
#include "postprocessing/ssrr.h"
#include "postprocessing/simpleblur.h"
#include "postprocessing/dof.h"
#include "postprocessing/fxaa.h"
#include "postprocessing/blurredeffect.h"
#include "postprocessing/volumetriclight.h"
#include "postprocessing/sobel.h"
#include "postprocessing/drawdefault.h"

#include "cubemapping/cubemap.h"
#include "cubemapping/texcubemap.h"
#include "texturing/envmap.h"
#include "cubemapping/envcubemap.h"
#include "cubemapping/irrmap.h"
#include "cubemapping/prefilterEnvmap.h"
#include "cubemapping/iblmap.h"
#include "cubemapping/skybox.h"
#include "cubemapping/reflectionprobe.h"
#include "texturing/brdfIntMap.h"
#include "renderscene.h"
#include "utility/rendertime.h"
#include "framebuffer/cubebuffer.h"
#include "cubemapping/cubemapfactory.h"

#include "guirenderer.h"
#include "elements/objectlister.h"
#include "snippets/postsnippets.h"
#include "snippets/blursnippets.h"
#include "elements/posteffectlister.h"
#include "elements/systeminformation.h"

#include "physics.h"
#include "worldphysics.h"
#include "rigidbody.h"
#include "animation/animator.h"
#include "animation/skeleton.h"

namespace geeL {

	using SceneInitialization = std::function<void(
		Application& app,
		DeferredRenderer& renderer,
		GUIRenderer& gui,
		RenderScene& scene,
		LightManager& lightManager, 
		TransformFactory& transformFactory, 
		MeshFactory& meshFactory,
		MaterialFactory& materialFactory,
		CubeMapFactory& cubeMapFactory,
		DefaultPostProcess& def,
		Physics* physics)>;


	class Configuration {

	public:
		Configuration(RenderWindow& window, PerspectiveCamera& camera, SceneInitialization initFunction,
			GBufferContent content = GBufferContent::Default);

		void run();

	private:
		RenderWindow& window;
		PerspectiveCamera& camera;
		SceneInitialization initFunction;
		GBufferContent content;

	};


}

#endif