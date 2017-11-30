#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>
#include "texturing/functionaltexture.h"

namespace geeL {

	class Camera;
	class RenderShader;
	class Texture;
	class TextureCube;

	enum class ShaderTransformSpace;


	//Cubemap that consists of six textures for each side
	class CubeMap : public FunctionalTexture {

	public:
		virtual void bindMap(const RenderShader& shader, std::string name) const;

		virtual void bind(const Camera& camera, const RenderShader& shader,
			const std::string& name, ShaderTransformSpace space) const;

		virtual void add(RenderShader& shader, std::string name) const;
		virtual void remove(RenderShader& shader) const;

		TextureCube& getTextureCube();

	protected:
		CubeMap(std::unique_ptr<TextureCube> texture);

	private:
		TextureCube& textureCube;

	};


	//Dynamic cube map that can be redrawn during runtime
	class DynamicCubeMap : public CubeMap {

	public:
		virtual ~DynamicCubeMap() {}

		virtual void draw() = 0;

	protected:
		DynamicCubeMap(std::unique_ptr<TextureCube> texture);

	};

}

#endif
