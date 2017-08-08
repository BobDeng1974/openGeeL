#include "shader/sceneshader.h"
#include "texturing/imagetexture.h"
#include "texturing/envmap.h"
#include "texturing/dynamictexture.h"
#include "framebuffer/gbuffer.h"
#include "genericmaterial.h"
#include "defaultmaterial.h"
#include "material.h"
#include "materialfactory.h"

using namespace std;

namespace geeL {

	MaterialFactory::MaterialFactory(const GBuffer& buffer, ShaderProvider* const provider) :
		forwardShader(new SceneShader("renderer/shaders/lighting.vert", FragmentShader("renderer/shaders/lighting.frag"), 
			ShaderTransformSpace::World)),
		deferredShader(new SceneShader("renderer/shaders/gbuffer.vert", FragmentShader(buffer.getFragmentPath(), false), 
			ShaderTransformSpace::View)),
		deferredAnimatedShader(new SceneShader("renderer/shaders/gbufferanim.vert", FragmentShader(buffer.getFragmentPath()),
			ShaderTransformSpace::View)), provider(provider) {

		forwardShader->mapOffset = 1;
		shaders.push_back(forwardShader);
	}

	MaterialFactory::~MaterialFactory() {
		delete deferredShader;
		delete deferredAnimatedShader;

		for (auto shader = shaders.begin(); shader != shaders.end(); shader++)
			delete *shader;

		for (auto material = container.begin(); material != container.end(); material++)
			delete *material;

		for (auto it = otherTextures.begin(); it != otherTextures.end(); it++) {
			Texture2D* tex = *it;
			delete tex;
		}

		for (auto it = textures.begin(); it != textures.end(); it++) {
			ImageTexture* tex = it->second;
			delete tex;
		}

		for (auto it = textureMaps.begin(); it != textureMaps.end(); it++) {
			TextureMap* tex = it->second;
			delete tex;
		}

		for (auto it = envMaps.begin(); it != envMaps.end(); it++) {
			EnvironmentMap* tex = it->second;
			delete tex;
		}
	}


	ImageTexture& MaterialFactory::CreateTexture(std::string filePath, ColorType colorType,
		WrapMode wrapMode, FilterMode filterMode, AnisotropicFilter filter) {

		if (textures.find(filePath) == textures.end())
			textures[filePath] = new ImageTexture(filePath.c_str(), colorType, wrapMode, filterMode, filter);

		return *textures[filePath];
	}

	TextureMap& MaterialFactory::CreateTextureMap(string filePath, MapType type, ColorType colorType, 
		WrapMode wrapMode, FilterMode filterMode, AnisotropicFilter filter) {
		
		if (textureMaps.find(filePath) == textureMaps.end())
			textureMaps[filePath] = new TextureMap(filePath.c_str(),  type, colorType, wrapMode, filterMode, filter);

		return *textureMaps[filePath];
	}

	DynamicRenderTexture& MaterialFactory::CreateDynamicRenderTexture(const Camera& camera, Resolution resolution) {
		DynamicRenderTexture* texture = new DynamicRenderTexture(camera, resolution, renderCall);
		otherTextures.push_back(texture);

		return *texture;
	}

	EnvironmentMap& MaterialFactory::CreateEnvironmentMap(string filePath) {
		if (envMaps.find(filePath) == envMaps.end())
			envMaps[filePath] = new EnvironmentMap(filePath);

		return *envMaps[filePath];
	}

	DefaultMaterialContainer& MaterialFactory::CreateMaterial() {
		DefaultMaterialContainer* mat = new DefaultMaterialContainer();
		container.push_back(mat);

		return *mat;
	}

	GenericMaterialContainer& MaterialFactory::CreateMaterial(RenderShader& shader) {
		GenericMaterialContainer* mat = new GenericMaterialContainer();
		container.push_back(mat);

		return *mat;
	}

	SceneShader& MaterialFactory::CreateShader(DefaultShading shading, string fragmentPath) {
		std::string vertexPath;

		switch (shading) {
			case DefaultShading::DeferredStatic:
				vertexPath = "renderer/shaders/gbuffer.vert";
				break;
			case DefaultShading::DeferredSkinned:
				vertexPath = "renderer/shaders/gbufferanim.vert";
				break;
			case DefaultShading::ForwardStatic:
				vertexPath = "renderer/shaders/lighting.vert";
				break;
			case DefaultShading::ForwardSkinned:
				vertexPath = "renderer/shaders/lighting.vert"; //TODO: create actual shader
				break;
		}

		FragmentShader frag = FragmentShader(fragmentPath);

		ShaderTransformSpace space = (shading == DefaultShading::DeferredSkinned) || (shading == DefaultShading::DeferredStatic) ?
			ShaderTransformSpace::View : ShaderTransformSpace::World;

		shaders.push_back(new SceneShader(vertexPath, frag, space, "camera", "skybox", provider));
		return *shaders.back();
	}

	SceneShader& MaterialFactory::CreateShader(std::string fragmentPath, bool animated) {
		std::string vertexPath = animated ? "renderer/shaders/lighting.vert"
			: "renderer/shaders/lighting.vert"; //TODO: create actual shader

		FragmentShader frag = FragmentShader(fragmentPath);

		shaders.push_back(new SceneShader(vertexPath, frag, ShaderTransformSpace::World, 
			"camera", "skybox", provider));

		return *shaders.back();
	}
	
	SceneShader& MaterialFactory::getDefaultShader(DefaultShading shading) const {
		switch (shading) {
			case DefaultShading::DeferredStatic:
				return *deferredShader;
			case DefaultShading::DeferredSkinned:
				return *deferredAnimatedShader;
			case DefaultShading::ForwardStatic:
				return *forwardShader;
			case DefaultShading::ForwardSkinned:
				//TODO: implement animated forward shader
				return *forwardShader;
		}

		return *deferredShader;
	}

	SceneShader& MaterialFactory::getForwardShader() const {
		return *forwardShader;
	}

	SceneShader& MaterialFactory::getDeferredShader() const {
		return *deferredShader;
	}

	void MaterialFactory::setRenderCall(RenderCall function) {
		renderCall = function;
	}

}