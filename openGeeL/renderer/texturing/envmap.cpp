#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "stb_image.h"
#include "envmap.h"

using namespace std;

namespace geeL {

	EnvironmentMap::EnvironmentMap(const string& fileName) : Texture2D(ColorType::RGB32) {

		stbi_set_flip_vertically_on_load(true);
		int width, height, nrComponents;
		float* image = stbi_loadf(fileName.c_str(), &width, &height, &nrComponents, 0);
		
		setResolution(width, height);

		if (image) {
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, image);

			initWrapMode(WrapMode::ClampEdge);
			initFilterMode(FilterMode::Linear);

			stbi_image_free(image);
		}
		else
			cout << "Failed to load environment map image.\n";

		stbi_set_flip_vertically_on_load(false);
	}

}