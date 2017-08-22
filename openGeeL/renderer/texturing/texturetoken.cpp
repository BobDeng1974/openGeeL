#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "texturetoken.h"

namespace geeL {

	inline TextureToken::TokenInner::~TokenInner() {
		glDeleteTextures(1, &id);

		//std::cout << "Delete " << id << "\n";
	}


	TextureToken::TextureToken() {
		unsigned int id = 0;
		glGenTextures(1, &id);

		token = std::make_shared<TokenInner>(id);
	}


	TextureToken& TextureToken::operator= (const TextureToken& other) {
		if (this != &other)
			token = other.token;

		return *this;
	}


}