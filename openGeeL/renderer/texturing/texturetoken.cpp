#define GLEW_STATIC
#include <glew.h>
#include "texturetoken.h"

namespace geeL {

	TextureToken::TokenInner::TokenInner(unsigned int id)
		: id(id) {}

	TextureToken::TokenInner::~TokenInner() {
		glDeleteTextures(1, &id);
	}

	TextureToken::TextureToken(const TextureToken& other)
		: token(other.token) {}

	TextureToken::TextureToken(TextureToken&& other)
		: token(other.token) {}


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

	TextureToken & TextureToken::operator=(TextureToken&& other) {
		if (this != &other)
			token = other.token;

		return *this;
	}


}