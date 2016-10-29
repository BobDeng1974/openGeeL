#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>

namespace geeL {

class Shader;

class CubeMap {

public:

	CubeMap(std::string rightPath, std::string leftPath, std::string topPath,
		std::string bottomPath, std::string backPath, std::string frontPath);

	void draw(const Shader& shader, std::string name);

private:
	unsigned int id;

};

}

#endif
