#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>

namespace geeL {

	class Camera;
	class RenderShader;
	class TextureCube;

	enum class ShaderTransformSpace;

	//Cubemap that consists of six textures for each side
	class CubeMap {

	public:
		virtual ~CubeMap() {}

		virtual void bindMap(const RenderShader& shader, std::string name) const;

		virtual void bind(const Camera& camera, const RenderShader& shader,
			const std::string& name, ShaderTransformSpace space) const;

		virtual void add(RenderShader& shader, std::string name) const;
		virtual void remove(RenderShader& shader) const;

		virtual unsigned int getID() const;

		const TextureCube& getTexture() const;
		TextureCube& getTexture();

	protected:
		CubeMap(TextureCube* texture) : texture(texture) {}

		TextureCube* texture;

	};


	//Dynamic cube map that can be redrawn during runtime
	class DynamicCubeMap : public CubeMap {

	public:
		virtual ~DynamicCubeMap() {}

		virtual void draw() = 0;

	protected:
		DynamicCubeMap(TextureCube* texture) : CubeMap(texture) {}

	};
}

#endif
