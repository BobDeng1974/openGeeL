#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include <vector>
#include "texture.h"

namespace geeL {

	class Texture3D : public Texture {

	public:
		Texture3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int levels);

		virtual unsigned int getID() const;
		virtual void remove();

	private:
		unsigned int id;
		std::vector<float> buffer;

	};


	inline unsigned int Texture3D::getID() const {
		return id;
	}

}

#endif
