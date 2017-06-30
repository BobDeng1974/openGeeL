#ifndef IMAGETEXTURECUBE_H
#define IMAGETEXTURECUBE_H

#include <string>
#include "texture.h"

namespace geeL {

	class RenderShader;

	//Cubemap that consists of six textures for each side
	class ImageTextureCube : public TextureCube {

	public:
		ImageTextureCube(const std::string& rightPath, const std::string& leftPath, const std::string& topPath,
			const std::string& bottomPath, const std::string& backPath, const std::string& frontPath, 
			WrapMode wrapMode = WrapMode::ClampEdge, FilterMode filterMode = FilterMode::Linear);
		~ImageTextureCube();

		virtual unsigned int getID() const;
		virtual void remove();

		virtual unsigned int getResolution() const;

	private:
		unsigned int id, resolution;

		ImageTextureCube(const ImageTextureCube& other) = delete;
		ImageTextureCube& operator= (const ImageTextureCube& other) = delete;

	};

	inline unsigned int ImageTextureCube::getID() const {
		return id;
	}

	inline unsigned int ImageTextureCube::getResolution() const {
		return resolution;
	}

}

#endif
