#include "stb_image.h"
#include "envmap.h"

using namespace std;

namespace geeL {
	
	EnvironmentMap::EnvironmentMap(const string& fileName) 
		: EnvironmentMap(ImageContainer(fileName)) {}

	EnvironmentMap::EnvironmentMap(ImageContainer&& container)
		: ImageTexture(ImageData(container.image), 
			container.width, 
			container.height, 
			ColorType::RGB32, 
			FilterMode::Linear, 
			WrapMode::ClampEdge, 
			AnisotropicFilter::None) {}


	EnvironmentMap::ImageContainer::ImageContainer(const string& fileName) {
		stbi_set_flip_vertically_on_load(true);
		image = stbi_loadf(fileName.c_str(), &width, &height, &nrComponents, 0);
		stbi_set_flip_vertically_on_load(false);
	}


}