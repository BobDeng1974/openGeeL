#include "stb_image.h"
#include "envmap.h"

using namespace std;

namespace geeL {
	
	EnvironmentMap::EnvironmentMap()
		: Texture2D(ColorType::RGB32) {}
	
	EnvironmentMap::EnvironmentMap(const string& fileName) 
		: EnvironmentMap(ImageContainer(fileName)) {}

	EnvironmentMap::EnvironmentMap(ImageContainer&& container) 
		: Texture2D(Resolution(container.width, container.height)
			, ColorType::RGB32
			, FilterMode::Linear
			, WrapMode::ClampEdge
			, container.image) {}


	EnvironmentMap::ImageContainer::ImageContainer(const string& fileName) {
		stbi_set_flip_vertically_on_load(true);
		image = stbi_loadf(fileName.c_str(), &width, &height, &nrComponents, 0);
	}

	EnvironmentMap::ImageContainer::~ImageContainer() {
		stbi_image_free(image);
		stbi_set_flip_vertically_on_load(false);
	}

}