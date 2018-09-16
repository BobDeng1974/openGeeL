#ifndef IMAGETEXTURECUBE_H
#define IMAGETEXTURECUBE_H

#include <string>
#include "texture.h"

namespace geeL {

	//Cubemap that consists of six textures for each side
	class ImageTextureCube : public TextureCube {

	public:
		ImageTextureCube(const std::string& rightPath, 
			const std::string& leftPath, 
			const std::string& topPath,
			const std::string& bottomPath, 
			const std::string& backPath, 
			const std::string& frontPath, 
			WrapMode wrapMode = WrapMode::ClampEdge, 
			FilterMode filterMode = FilterMode::Linear);

	private:
		struct ImageContainer {
			int width, height;
			void* images[6];

			ImageContainer(const std::string& rightPath, 
				const std::string& leftPath, 
				const std::string& topPath,
				const std::string& bottomPath, 
				const std::string& backPath, 
				const std::string& frontPath);

			~ImageContainer();
		};

		ImageTextureCube(ImageContainer&& container, 
			WrapMode wrapMode, 
			FilterMode filterMode);

		ImageTextureCube(const ImageTextureCube& other) = delete;
		ImageTextureCube& operator= (const ImageTextureCube& other) = delete;

	};

}

#endif
