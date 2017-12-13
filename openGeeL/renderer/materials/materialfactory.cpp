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
	}


	MemoryObject<ImageTexture> MaterialFactory::createTexture(std::string filePath, ColorType colorType,
		FilterMode filterMode, WrapMode wrapMode,  AnisotropicFilter filter) {

		auto it = textures.find(filePath);
		//Create new texture if none exist yet
		if (it == textures.end()
			//or memory of existing one expired
			|| (it != textures.end() && it->second.expired())) {

			MemoryObject<ImageTexture> spTexture(
				new ImageTexture(filePath.c_str(), colorType, wrapMode, filterMode, filter), [this](ImageTexture* t) {
					//Add a custom deleter to provide a callback when resource gets destroyed
					this->onRemove(*t);
					delete t;
			});

			weak_ptr<ImageTexture> wpTexture(spTexture);
			onAdd(*spTexture);

			textures[filePath] = wpTexture;
			return spTexture;
		}

		weak_ptr<ImageTexture> tex = it->second;
		return MemoryObject<ImageTexture>(tex.lock());
	}

	MemoryObject<TextureMap> MaterialFactory::createTextureMap(string filePath, MapType type, ColorType colorType,
		FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter filter) {
		
		auto it = textureMaps.find(filePath);
		//Create new texture if none exist yet
		if (it == textureMaps.end()
			//or memory of existing one expired
			|| (it != textureMaps.end() && it->second.expired())) {

			MemoryObject<TextureMap> spTexture(
				new TextureMap(filePath.c_str(), type, colorType, wrapMode, filterMode, filter), [this](TextureMap* m) {
					//Add a custom deleter to provide a callback when resource gets destroyed
					this->onRemove(*m);
					delete m;
			});

			weak_ptr<TextureMap> wpTexture(spTexture);
			onAdd(*spTexture);

			textureMaps[filePath] = wpTexture;
			return spTexture;
		}

		weak_ptr<TextureMap> tex = it->second;
		return MemoryObject<TextureMap>(tex.lock());
	}


	MemoryObject<DefaultMaterialContainer> MaterialFactory::createDefaultMaterial() {
		return MemoryObject<DefaultMaterialContainer>(new DefaultMaterialContainer());;
	}

	MemoryObject<GenericMaterialContainer> MaterialFactory::createGenericMaterial() {
		return MemoryObject<GenericMaterialContainer>(new GenericMaterialContainer());;
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