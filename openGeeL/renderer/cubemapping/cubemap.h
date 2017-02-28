#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>

namespace geeL {

	class Shader;

	//Cubemap that consists of six textures for each side
	class CubeMap {

	public:
		virtual void draw(const Shader& shader, std::string name) const = 0;

		unsigned int getID() const {
			return id;
		}

	protected:
		unsigned int id;

	};
}

#endif
