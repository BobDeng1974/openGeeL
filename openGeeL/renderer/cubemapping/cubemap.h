#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>

using namespace std;

namespace geeL {

	class Shader;

	class CubeMap {

	public:

		CubeMap(string rightPath, string leftPath, string topPath, 
			string bottomPath, string backPath, string frontPath);

		void draw(const Shader& shader, string name);

	private:
		unsigned int id;

	};


}


#endif
