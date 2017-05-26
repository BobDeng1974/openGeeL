#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>

namespace geeL {

	class Camera;
	class Shader;

	enum class ShaderTransformSpace;

	//Cubemap that consists of six textures for each side
	class CubeMap {

	public:
		virtual void draw(const Shader& shader, std::string name) const;

		virtual void bind(const Camera& camera, const Shader& shader,
			const std::string& name, ShaderTransformSpace space) const {}

		virtual void add(Shader& shader, std::string name) const;
		virtual void remove(Shader& shader, unsigned int id) const;

		virtual unsigned int getID() const;

	protected:
		unsigned int id;

	};


	//Dynamic cube map that can be redrawn during runtime
	class DynamicCubeMap : public CubeMap {

	public:
		virtual void update() = 0;

	};
}

#endif
