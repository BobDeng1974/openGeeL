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
	class SceneShader;
	class ShaderProvider;


	class MaterialFactory {

	public:
		MaterialFactory(const GBuffer& buffer, ShaderProvider* const provider = nullptr);
		~MaterialFactory();

		//Creates and returns a new texture from given file path or 
		//returns an existing texture if it already uses this file
		ImageTexture& CreateTexture(std::string filePath, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::None, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		//Creates and returns a new texture map from given file path or 
		//returns an existing texture if it already uses this file
		TextureMap& CreateTextureMap(std::string filePath,
			MapType type = MapType::Diffuse, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::None,
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		//Create dynamic render texture that can render scene from given camera's point of view.
		//Note: Returned texture won't get drawn automatically and needs to be attached to an appropriate renderer
		DynamicRenderTexture& CreateDynamicRenderTexture(const Camera& camera, Resolution resolution);

		//Create and returns new environment map or returns existing one if file is already in use
		EnvironmentMap& CreateEnvironmentMap(std::string filePath);

		//Creates and returns a new material with default shading
		DefaultMaterialContainer& CreateMaterial();

		//Creates and returns a new materials with given shading
		GenericMaterialContainer& CreateMaterial(RenderShader& shader);

		//Creates and returns a new shader program with given file paths
		//for vertex and fragment shaders
		SceneShader& CreateShader(ShadingMethod shading, std::string fragmentPath);
		SceneShader& CreateShader(std::string fragmentPath, bool animated = false);

		SceneShader& getDefaultShader(ShadingMethod shading) const;

		//Returns default shader for static forward rendering
		SceneShader& getForwardShader() const;

		//Returns default shader for static deferred rendering
		SceneShader& getDeferredShader() const;

	private:
		ShaderProvider* const provider;

		SceneShader* forwardShader;
		SceneShader* deferredShader;
		SceneShader* deferredAnimatedShader;

		std::list<MaterialContainer*> container;
		std::list<SceneShader*> shaders;
		std::list<Texture2D*> otherTextures;
		std::map<std::string, ImageTexture*> textures;
		std::map<std::string, TextureMap*> textureMaps;
		std::map<std::string, EnvironmentMap*> envMaps;

	};
}

#endif
