#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include <functional>
#include <list>
#include <map>
#include <string>
#include "memory/memoryobject.h"
#include "renderer/glstructures.h"
#include "texturing/imagetexture.h"
#include "utility/listener.h"
#include "shader/defshading.h"

using namespace geeL::memory;

namespace geeL {

	class Camera;
	class FrameBuffer;
	class GBuffer;
	class ITextureProvider;
	class Material;
	class MaterialContainer;
	class DefaultMaterialContainer;
	class GenericMaterialContainer;
	class RenderShader;
	class SceneShader;
	class ShaderProvider;


	class MaterialFactory : public DataEventActuator<GLStructure> {

	public:
		MaterialFactory(const GBuffer& buffer, ITextureProvider& textureProvider, 
			ShaderProvider* const provider = nullptr);
		~MaterialFactory();

		//Creates and returns a new texture from given file path or 
		//returns an existing texture if it already uses this file
		MemoryObject<ImageTexture> createTexture(std::string filePath,
			ColorType colorType = ColorType::RGBA,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		//Creates and returns a new texture map from given file path or 
		//returns an existing texture if it already uses this file
		MemoryObject<TextureMap> createTextureMap(std::string filePath,
			MapType type = MapType::Diffuse, 
			ColorType colorType = ColorType::RGBA,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);


		//Creates and returns a new material with default shading
		MemoryObject<DefaultMaterialContainer> createDefaultMaterial();

		//Creates and returns a new materials with given shading
		MemoryObject<GenericMaterialContainer> createGenericMaterial();

		//Creates and returns a new shader program from given fragment path
		//Note: Only generic, forward and transparent shading methods allowed
		SceneShader& createShader(ShadingMethod shading, std::string fragmentPath, bool animated = false);

		SceneShader& getDefaultShader(ShadingMethod shading, bool animated);

	private:
		ITextureProvider& textureProvider;
		ShaderProvider* const provider;

		SceneShader* genericShader;
		SceneShader* genericAnimatedShader;
		SceneShader* forwardShader;
		SceneShader* forwardAnimatedShader;
		SceneShader* deferredShader;
		SceneShader* deferredAnimatedShader;
		SceneShader* transparentShader;
		SceneShader* transparentAnimatedShader;

		std::list<SceneShader*> shaders;
		std::map<std::string, std::weak_ptr<ImageTexture>> textures;
		std::map<std::string, std::weak_ptr<TextureMap>> textureMaps;

	};
}

#endif
