#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>

namespace geeL {

	class Shader;

	//Cubemap that consists of six textures for each side
	class CubeMap {

	public:
		virtual void bind(const Shader& shader, std::string name) const = 0;

		virtual void add(Shader& shader, std::string name) const;
		virtual unsigned int getID() const;

	protected:
		unsigned int id;

	};
}

#endif
