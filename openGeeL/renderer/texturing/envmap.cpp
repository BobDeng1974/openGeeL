#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "stb_image.h"
#include "envmap.h"

using namespace std;

namespace geeL {

	EnvironmentMap::EnvironmentMap(string fileName) {

		stbi_set_flip_vertically_on_load(true);
		int width, height, nrComponents;
		float *image = stbi_loadf(fileName.c_str(), &width, &height, &nrComponents, 0);
		
		if (image) {
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, image);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(image);
		}
		else
			cout << "Failed to load environment map image.\n";

		stbi_set_flip_vertically_on_load(false);
	}


	const unsigned int EnvironmentMap::GetID() const {
		return id;
	}

}