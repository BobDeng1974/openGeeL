#include <iostream>
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
		genericShader(new SceneShader("renderer/shaders/lighting.vert", FragmentShader("renderer/shaders/lighting.frag"),
			ShaderTransformSpace::World, ShadingMethod::Generic)),
		forwardShader(new SceneShader("renderer/shaders/lighting.vert", FragmentShader("renderer/shaders/lighting.frag"), 
			ShaderTransformSpace::World, ShadingMethod::Forward)),
		deferredShader(new SceneShader("renderer/shaders/gbuffer.vert", FragmentShader(buffer.getFragmentPath(), false), 
			ShaderTransformSpace::View, ShadingMethod::Deferred)),
		deferredAnimatedShader(new SceneShader("renderer/shaders/gbufferanim.vert", FragmentShader(buffer.getFragmentPath()),
			ShaderTransformSpace::View, ShadingMethod::DeferredSkinned)), provider(provider) {

		genericShader->mapOffset = 1;
		forwardShader->mapOffset = 1;

		shaders.push_back(genericShader);
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
		DynamicRenderTexture* texture = new DynamicRenderTexture(camera, resolution);
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

	SceneShader& MaterialFactory::CreateShader(ShadingMethod shading, string fragmentPath) {
		std::string vertexPath;
		ShaderTransformSpace space;

		switch (shading) {
			case ShadingMethod::Generic:
				vertexPath = "renderer/shaders/lighting.vert";
				space = ShaderTransformSpace::World;
				break;
			case ShadingMethod::GenericSkinned:
				vertexPath = "renderer/shaders/lighting.vert"; //TODO: create actual shader
				space = ShaderTransformSpace::World;
				break;
			case ShadingMethod::Forward:
				vertexPath = "renderer/shaders/lighting.vert";
				space = ShaderTransformSpace::View;
				break;
			case ShadingMethod::ForwardSkinned:
				vertexPath = "renderer/shaders/lighting.vert"; //TODO: create actual shader
				space = ShaderTransformSpace::View;
				break;
			default:
				throw "This shading method isn't allowed\n";
		}

		FragmentShader frag = FragmentShader(fragmentPath);

		shaders.push_back(new SceneShader(vertexPath, frag, space, shading, provider));
		return *shaders.back();
	}

	SceneShader& MaterialFactory::getDefaultShader(ShadingMethod shading) const {
		switch (shading) {
			case ShadingMethod::Generic:
				return *genericShader;
			case ShadingMethod::GenericSkinned:
				return *genericShader; //TODO: create actual shader
			case ShadingMethod::Deferred:
				return *deferredShader;
			case ShadingMethod::DeferredSkinned:
				return *deferredAnimatedShader;
			case ShadingMethod::Forward:
				return *forwardShader;
			case ShadingMethod::ForwardSkinned:
				return *forwardShader; //TODO: create actual shader
		}

		return *deferredShader;
	}

	SceneShader& MaterialFactory::getDeferredShader() const {
		return *deferredShader;
	}

}