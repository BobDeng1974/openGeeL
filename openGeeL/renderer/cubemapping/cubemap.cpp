#define GLEW_STATIC
#include <glew.h>
#include "texturing/texture.h"
#include "shader/rendershader.h"
#include "shader/bindingstack.h"
#include "cubemap.h"

#define GL_TEXTURE_CUBE_MAP 0x8513

namespace geeL {
	
	CubeMap::CubeMap(std::unique_ptr<TextureCube> texture)
		: FunctionalTexture(std::unique_ptr<Texture>(texture.get())) 
		, textureCube(*texture.release()) {}


	void CubeMap::bindMap(const RenderShader& shader, std::string name) const {
		TextureBindingStack::bindSingleTexture(getID(), shader, 1, name, TextureType::TextureCube);
	}

	void CubeMap::bind(const Camera& camera, const RenderShader& shader, 
		const std::string& name, ShaderTransformSpace space) const {

		shader.bind<int>(name + "global", true);
	}

	void CubeMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(getTexture(), name + "albedo");
	}

	void CubeMap::remove(RenderShader& shader) const {
		shader.removeMap(getTexture());
	}

	TextureCube& CubeMap::getTextureCube() {
		return textureCube;
	}


	DynamicCubeMap::DynamicCubeMap(std::unique_ptr<TextureCube> texture)
		: CubeMap(std::move(texture)) {}

}