#ifndef TEXTURE_H
#define TEXTURE_H

namespace geeL {

class Shader;

	class Texture {

	public:
		virtual unsigned int getID() const = 0;

	};
}

#endif
