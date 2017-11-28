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

	MaterialFactory::MaterialFactory(const GBuffer& buffer, ShaderProvider* const provider) 
		: deferredShader(new SceneShader("renderer/shaders/gbuffer.vert", 
			FragmentShader(buffer.getFragmentPath(), false), 
			ShaderTransformSpace::View, 
			ShadingMethod::Deferred))
		, deferredAnimatedShader(new SceneShader("renderer/shaders/gbufferAnim.vert", 
			FragmentShader(buffer.getFragmentPath(), false),
			ShaderTransformSpace::View, 
			ShadingMethod::Deferred, true))
		, genericShader(nullptr)
		, genericAnimatedShader(nullptr)
		, forwardShader(nullptr)
		, forwardAnimatedShader(nullptr)
		, transparentODShader(nullptr)
		, transparentODAnimatedShader(nullptr)
		, transparentOIDShader(nullptr)
		, provider(provider) {}

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

	SceneShader& MaterialFactory::CreateShader(ShadingMethod shading, string fragmentPath, bool animated) {
		std::string vertexPath;
		ShaderTransformSpace space;

		switch (shading) {
			case ShadingMethod::Generic:
				vertexPath = animated 
					? "renderer/shaders/lightingAnim.vert" 
					: "renderer/shaders/lighting.vert";

				space = ShaderTransformSpace::World;
				break;
			case ShadingMethod::Forward:
			case ShadingMethod::TransparentOD:
			case ShadingMethod::TransparentOID:
				vertexPath = animated 
					? "renderer/shaders/forwardAnim.vert" 
					: "renderer/shaders/forwardlighting.vert";

				space = ShaderTransformSpace::View;
				break;
			default:
				throw "This shading method isn't allowed\n";
		}

		FragmentShader frag = FragmentShader(fragmentPath);

		shaders.push_back(new SceneShader(vertexPath, frag, space, shading, provider));
		return *shaders.back();
	}

	SceneShader& MaterialFactory::getDefaultShader(ShadingMethod shading, bool animated) {
		switch (shading) {
			case ShadingMethod::Generic:
				if (animated) {
					if (genericAnimatedShader == nullptr) {
						genericAnimatedShader = new SceneShader("renderer/shaders/lightingAnim.vert",
							FragmentShader("renderer/shaders/lighting.frag"),
							ShaderTransformSpace::World,
							ShadingMethod::Generic,
							true);

						genericAnimatedShader->mapOffset = 1;
						shaders.push_back(genericAnimatedShader);

					}

					return *genericAnimatedShader;
				}
				else {
					if (genericShader == nullptr) {
						genericShader = new SceneShader("renderer/shaders/lighting.vert",
							FragmentShader("renderer/shaders/lighting.frag"),
							ShaderTransformSpace::World,
							ShadingMethod::Generic, 
							false);

						genericShader->mapOffset = 1;
						shaders.push_back(genericShader);
					}

					return *genericShader;
				}
			case ShadingMethod::Deferred:
				return animated ? *deferredAnimatedShader : *deferredShader;
			case ShadingMethod::Forward:
				if (animated) {
					if (forwardAnimatedShader == nullptr) {
						forwardAnimatedShader = new SceneShader("renderer/shaders/forwardAnim.vert",
							FragmentShader("renderer/shaders/forwardlighting.frag"),
							ShaderTransformSpace::View,
							ShadingMethod::Forward,
							true);

						forwardAnimatedShader->mapOffset = 1;
						shaders.push_back(forwardAnimatedShader);
					}

					return *forwardAnimatedShader;
				}
				else{
					if (forwardShader == nullptr) {
						forwardShader = new SceneShader("renderer/shaders/forwardlighting.vert",
							FragmentShader("renderer/shaders/forwardlighting.frag"),
							ShaderTransformSpace::View,
							ShadingMethod::Forward, 
							false);

						forwardShader->mapOffset = 1;
						shaders.push_back(forwardShader);
					}

					return *forwardShader;
				}
			case ShadingMethod::TransparentOD:
				if (animated) {
					if (transparentODAnimatedShader == nullptr) {
						transparentODAnimatedShader = new SceneShader("renderer/shaders/forwardAnim.vert",
							FragmentShader("renderer/shaders/forwardlighting.frag"),
							ShaderTransformSpace::View,
							ShadingMethod::TransparentOD,
							true);

						transparentODAnimatedShader->mapOffset = 1;
						shaders.push_back(transparentODAnimatedShader);
					}

					return *transparentODAnimatedShader;
				}
				else {
					if (transparentODShader == nullptr) {
						transparentODShader = new SceneShader("renderer/shaders/forwardlighting.vert",
							FragmentShader("renderer/shaders/forwardlighting.frag"),
							ShaderTransformSpace::View,
							ShadingMethod::TransparentOD,
							false);

						transparentODShader->mapOffset = 1;
						shaders.push_back(transparentODShader);
					}

					return *transparentODShader;
				}
			case ShadingMethod::TransparentOID:
				if (transparentOIDShader == nullptr) {
					transparentOIDShader = new SceneShader("renderer/shaders/forwardlighting.vert",
						FragmentShader("renderer/shaders/transparentlighting.frag"),
						ShaderTransformSpace::View,
						ShadingMethod::TransparentOID, 
						false);

					transparentOIDShader->mapOffset = 1;
					shaders.push_back(transparentOIDShader);
				}

				return *transparentOIDShader;
		}

		return *deferredShader;
	}

	SceneShader& MaterialFactory::getDeferredShader() const {
		return *deferredShader;
	}

}