#include "texturing/texture.h"
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

		//mapOffset = 1;

		const ITexture& t1 = provider.requestDummy2D();
		const ITexture& t2 = provider.requestDummyCube();
		addMap(t1, "BRDFIntegrationMap");
		addMap(t2, "skybox.irradiance");
		addMap(t2, "skybox.prefilterEnv");
	}

	std::string TransparentLighting::removeMap(const ITexture& texture) {
		std::string name(Shader::removeMap(texture));
		addTextures(name);

		return name;
	}

	void TransparentLighting::removeMap(const std::string& name) {
		SceneShader::removeMap(name);
		addTextures(name);
	}


	void TransparentLighting::addTextures(const std::string& name) {
		if (name == "BRDFIntegrationMap")
			addMap(provider.requestDummy2D(), name);
		else if (name == "skybox.irradiance" || name == "skybox.prefilterEnv")
			addMap(provider.requestDummyCube(), name);
	}

}