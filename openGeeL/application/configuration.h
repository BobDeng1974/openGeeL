#ifndef GEELCONFIGURATION_H
#define GEELCONFIGURATION_H

#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <functional>

#include "application.h"
#include "threading.h"
#include "inputmanager.h"
#include "window.h"
#include "pipeline.h"
#include "renderscene.h"

#include "animation/animator.h"
#include "animation/skeleton.h"

#include "cameras/camera.h"
#include "cameras/perspectivecamera.h"
#include "cameras/movablecamera.h"

#include "cubemapping/cubemap.h"
#include "cubemapping/texcubemap.h"
#include "cubemapping/envcubemap.h"
#include "cubemapping/irrmap.h"
#include "cubemapping/prefilterEnvmap.h"
#include "cubemapping/iblmap.h"
#include "cubemapping/skybox.h"
#include "cubemapping/reflectionprobe.h"
#include "cubemapping/cubemapfactory.h"

#include "framebuffer/cubebuffer.h"
#include "framebuffer/gbuffer.h"

#include "lights/light.h"
#include "lights/lightmanager.h"
#include "lights/pointlight.h"
#include "lights/directionallight.h"
#include "lights/spotlight.h"

#include "lighting/deferredlighting.h"
#include "lighting/ibl.h"
#include "lighting/conetracer.h"

#include "materials/material.h"
#include "materials/defaultmaterial.h"
#include "materials/genericmaterial.h"
#include "materials/materialfactory.h"

#include "meshes/mesh.h"
#include "meshes/model.h"
#include "meshes/meshrenderer.h"
#include "meshes/skinnedrenderer.h"
#include "meshes/meshfactory.h"

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
#include "postprocessing/lensflare.h"
#include "postprocessing/motionblur.h"

#include "renderer/rendercontext.h"
#include "renderer/deferredrenderer.h"

#include "texturing/brdfIntMap.h"
#include "texturing/envmap.h"
#include "texturing/texture.h"
#include "texturing/imagetexture.h"
#include "texturing/layeredtexture.h"

#include "transformation/transform.h"
#include "transformation/threadedtransform.h"
#include "transformation/transformfactory.h"

#include "voxelization/voxelizer.h"
#include "voxelization/voxeloctree.h"
#include "voxelization/voxeltexture.h"

#include "guirenderer.h"
#include "elements/objectlister.h"
#include "snippets/postsnippets.h"
#include "snippets/blursnippets.h"
#include "elements/posteffectlister.h"
#include "elements/systeminformation.h"

#include "physics.h"
#include "worldphysics.h"
#include "rigidbody.h"


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
		Physics& physics)>;

	enum class PhysicsType {
		None,
		World
	};


	class Configuration {

	public:
		Configuration(RenderWindow& window, SceneInitialization initFunction,
			GBufferContent content = GBufferContent::Default, PhysicsType physicsType = PhysicsType::None);

		void run();

	private:
		RenderWindow& window;
		SceneInitialization initFunction;
		GBufferContent content;
		PhysicsType physicsType;

	};


}

#endif
