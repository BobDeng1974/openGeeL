#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>
#include "texturing/imagetexture.h"
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
		ImageTexture& createTexture(std::string filePath, 
			ColorType colorType = ColorType::RGBA,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		//Creates and returns a new texture map from given file path or 
		//returns an existing texture if it already uses this file
		TextureMap& createTextureMap(std::string filePath,
			MapType type = MapType::Diffuse, 
			ColorType colorType = ColorType::RGBA,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);


		//Creates and returns a new material with default shading
		std::shared_ptr<DefaultMaterialContainer> createDefaultMaterial();

		//Creates and returns a new materials with given shading
		std::shared_ptr<GenericMaterialContainer> createGenericMaterial();

		//Creates and returns a new shader program from given fragment path
		//Note: Only generic, forward and transparent shading methods allowed
		SceneShader& createShader(ShadingMethod shading, std::string fragmentPath, bool animated = false);

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

		std::list<SceneShader*> shaders;
		std::map<std::string, ImageTexture*> textures;
		std::map<std::string, TextureMap*> textureMaps;

	};
}

#endif
