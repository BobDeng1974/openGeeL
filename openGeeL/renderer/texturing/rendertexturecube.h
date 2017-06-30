#ifndef TEXTURECUBE_H
#define TEXTURECUBE_H

#include "texture.h"

namespace geeL {

	//3D Texture that is intended to be rendered into
	class RenderTextureCube : public TextureCube {

	public:
		RenderTextureCube() : TextureCube(ColorType::Single), id(0) {}
		RenderTextureCube(unsigned int resolution,
			WrapMode wrapMode = WrapMode::ClampEdge, FilterMode filterMode = FilterMode::Linear);
		~RenderTextureCube();

		virtual unsigned int getID() const;
		virtual void remove();

		virtual unsigned int getResolution() const;

	private:
		unsigned int id, resolution;

		RenderTextureCube(const RenderTextureCube& other) = delete;
		RenderTextureCube& operator= (const RenderTextureCube& other) = delete;

	};


	inline unsigned int RenderTextureCube::getID() const {
		return id;
	}

	inline unsigned int RenderTextureCube::getResolution() const {
		return resolution;
	}
	
}

#endif