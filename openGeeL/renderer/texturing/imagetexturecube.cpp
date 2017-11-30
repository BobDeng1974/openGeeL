#include "stb_image.h"
#include "imagetexturecube.h"

using namespace std;

namespace geeL {

	ImageTextureCube::ImageTextureCube(const string& rightPath, 
		const string& leftPath, 
		const string& topPath,
		const string& bottomPath, 
		const string& backPath, 
		const string& frontPath, 
		WrapMode wrapMode, 
		FilterMode filterMode) 
			: ImageTextureCube(ImageContainer(
					rightPath, leftPath, 
					topPath, bottomPath, 
					backPath, frontPath), 
				wrapMode, 
				filterMode) {}

	ImageTextureCube::ImageTextureCube(ImageContainer&& container, 
		WrapMode wrapMode,
		FilterMode filterMode)
			: TextureCube((container.width + container.height) / 2,
				ColorType::GammaSpace, 
				filterMode, 
				wrapMode, 
				container.images) {}


	ImageTextureCube::ImageContainer::ImageContainer(const std::string& rightPath, 
		const std::string& leftPath, 
		const std::string& topPath, 
		const std::string& bottomPath, 
		const std::string& backPath, 
		const std::string& frontPath) {

		images[0] = stbi_load(rightPath.c_str(), &width, &height, 0, STBI_rgb);
		images[1] = stbi_load(leftPath.c_str(), &width, &height, 0, STBI_rgb);
		images[2] = stbi_load(topPath.c_str(), &width, &height, 0, STBI_rgb);
		images[3] = stbi_load(bottomPath.c_str(), &width, &height, 0, STBI_rgb);
		images[4] = stbi_load(backPath.c_str(), &width, &height, 0, STBI_rgb);
		images[5] = stbi_load(frontPath.c_str(), &width, &height, 0, STBI_rgb);
	}

	ImageTextureCube::ImageContainer::~ImageContainer() {
		for (unsigned int i = 0; i < 6; i++)
			stbi_image_free(images[i]);
	}

}