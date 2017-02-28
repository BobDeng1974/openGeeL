#ifndef TEXTUREDCUBEMAP_H
#define TEXTUREDCUBEMAP_H

#include <string>
#include "cubemap.h"

namespace geeL {

	class Shader;

	//Cubemap that consists of six textures for each side
	class TexturedCubeMap : public CubeMap {

	public:
		TexturedCubeMap(std::string rightPath, std::string leftPath, std::string topPath,
			std::string bottomPath, std::string backPath, std::string frontPath);

		virtual void draw(const Shader& shader, std::string name) const;

	};
}

#endif
