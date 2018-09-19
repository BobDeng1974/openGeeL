#include "texturing/textureprovider.h"
#include "lightingshaders.h"

namespace geeL {

	ForwardLighting::ForwardLighting(bool animated) 
		: SceneShader(animated ? "shaders/lighting/lightingAnim.vert" : "shaders/lighting/lighting.vert",
			FragmentShader("shaders/lighting/lighting.frag"),
			ShaderTransformSpace::World,
			ShadingMethod::Forward,
			animated) {

		mapOffset = 1;
	}


	HybridLighting::HybridLighting(bool animated)
		: SceneShader(animated ? "shaders/lighting/forwardAnim.vert" : "shaders/lighting/forwardlighting.vert",
			FragmentShader("shaders/lighting/forwardlighting.frag"),
			ShaderTransformSpace::View,
			ShadingMethod::Hybrid,
			animated) {

		mapOffset = 1;
	}


	TransparentLighting::TransparentLighting(ITextureProvider& provider, bool animated)
		: SceneShader(animated ? "shaders/lighting/forwardAnim.vert" : "shaders/lighting/forwardlighting.vert",
			FragmentShader("shaders/lighting/transparentlighting.frag"),
			ShaderTransformSpace::View,
			ShadingMethod::Transparent,
			animated)
		, provider(provider) {

		mapOffset = 1;
	}

}