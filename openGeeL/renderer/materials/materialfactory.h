#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include <functional>
#include <list>
#include <map>
#include <string>
#include "texturing/imagetexture.h"
#include "texturing/envmap.h"
#include "shader/defshading.h"

namespace geeL {

	class Camera;
	class DynamicRenderTexture;
	class FrameBuffer;
	class GBuffer;
	class Material;
	class MaterialContainer;
	class DefaultMaterialContainer;
	class GenericMaterialContainer;
	class RenderShader;
	class SceneShader;
	class ShaderProvider;


	class MaterialFactory {

	public:
		MaterialFactory(const GBuffer& buffer, ShaderProvider* const provider = nullptr);
		~MaterialFactory();

		//Creates and returns a new texture from given file path or 
		//returns an existing texture if it already uses this file
		ImageTexture& CreateTexture(std::string filePath, 
			ColorType colorType = ColorType::RGBA,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		//Creates and returns a new texture map from given file path or 
		//returns an existing texture if it already uses this file
		TextureMap& CreateTextureMap(std::string filePath,
			MapType type = MapType::Diffuse, 
			ColorType colorType = ColorType::RGBA,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);


		//Creates and returns a new material with default shading
		DefaultMaterialContainer& createDefaultMaterial();

		//Creates and returns a new materials with given shading
		GenericMaterialContainer& createGenericMaterial();

		//Creates and returns a new shader program from given fragment path
		//Note: Only generic, forward and transparent shading methods allowed
		SceneShader& CreateShader(ShadingMethod shading, std::string fragmentPath, bool animated = false);

		//Returns default shader for static deferred rendering
		SceneShader& getDeferredShader() const;
		SceneShader& getDefaultShader(ShadingMethod shading, bool animated = false);

	private:
		ShaderProvider* const provider;

		SceneShader* genericShader;
		SceneShader* genericAnimatedShader;
		SceneShader* forwardShader;
		SceneShader* forwardAnimatedShader;
		SceneShader* deferredShader;
		SceneShader* deferredAnimatedShader;
		SceneShader* transparentODShader;
		SceneShader* transparentODAnimatedShader;
		SceneShader* transparentOIDShader;

		std::list<MaterialContainer*> container;
		std::list<SceneShader*> shaders;
		std::list<Texture2D*> otherTextures;
		std::map<std::string, ImageTexture*> textures;
		std::map<std::string, TextureMap*> textureMaps;

	};
}

#endif
