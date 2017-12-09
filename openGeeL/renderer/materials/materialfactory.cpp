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
		: deferredShader(new SceneShader("shaders/gbuffer.vert", 
			FragmentShader(buffer.getFragmentPath(), false), 
			ShaderTransformSpace::View, 
			ShadingMethod::Deferred))
		, deferredAnimatedShader(new SceneShader("shaders/gbufferAnim.vert", 
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

		for (auto it = textures.begin(); it != textures.end(); it++) {
			ImageTexture* tex = it->second;
			delete tex;
		}

		for (auto it = textureMaps.begin(); it != textureMaps.end(); it++) {
			TextureMap* tex = it->second;
			delete tex;
		}

	}


	ImageTexture& MaterialFactory::createTexture(std::string filePath, ColorType colorType,
		FilterMode filterMode, WrapMode wrapMode,  AnisotropicFilter filter) {

		if (textures.find(filePath) == textures.end())
			textures[filePath] = new ImageTexture(filePath.c_str(), colorType, wrapMode, filterMode, filter);

		return *textures[filePath];
	}

	TextureMap& MaterialFactory::createTextureMap(string filePath, MapType type, ColorType colorType, 
		FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter filter) {
		
		if (textureMaps.find(filePath) == textureMaps.end())
			textureMaps[filePath] = new TextureMap(filePath.c_str(), type, colorType, wrapMode, filterMode, filter);

		return *textureMaps[filePath];
	}


	shared_ptr<DefaultMaterialContainer> MaterialFactory::createDefaultMaterial() {
		return shared_ptr<DefaultMaterialContainer>(new DefaultMaterialContainer());;
	}

	shared_ptr<GenericMaterialContainer> MaterialFactory::createGenericMaterial() {
		return shared_ptr<GenericMaterialContainer>(new GenericMaterialContainer());;
	}

	SceneShader& MaterialFactory::createShader(ShadingMethod shading, string fragmentPath, bool animated) {
		std::string vertexPath;
		ShaderTransformSpace space;

		switch (shading) {
			case ShadingMethod::Generic:
				vertexPath = animated 
					? "shaders/lighting/lightingAnim.vert" 
					: "shaders/lighting/lighting.vert";

				space = ShaderTransformSpace::World;
				break;
			case ShadingMethod::Forward:
			case ShadingMethod::TransparentOD:
			case ShadingMethod::TransparentOID:
				vertexPath = animated 
					? "shaders/lighting/forwardAnim.vert" 
					: "shaders/lighting/forwardlighting.vert";

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
						genericAnimatedShader = new SceneShader("shaders/lighting/lightingAnim.vert",
							FragmentShader("shaders/lighting/lighting.frag"),
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
						genericShader = new SceneShader("shaders/lighting/lighting.vert",
							FragmentShader("shaders/lighting/lighting.frag"),
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
						forwardAnimatedShader = new SceneShader("shaders/lighting/forwardAnim.vert",
							FragmentShader("shaders/lighting/forwardlighting.frag"),
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
						forwardShader = new SceneShader("shaders/lighting/forwardlighting.vert",
							FragmentShader("shaders/lighting/forwardlighting.frag"),
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
						transparentODAnimatedShader = new SceneShader("shaders/lighting/forwardAnim.vert",
							FragmentShader("shaders/lighting/forwardlighting.frag"),
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
						transparentODShader = new SceneShader("shaders/lighting/forwardlighting.vert",
							FragmentShader("shaders/lighting/forwardlighting.frag"),
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
					transparentOIDShader = new SceneShader("shaders/lighting/forwardlighting.vert",
						FragmentShader("shaders/lighting/transparentlighting.frag"),
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