#define GLEW_STATIC
#include <glew.h>
#include "fbotoken.h"

namespace geeL {


	RenderBufferToken::TokenInner::~TokenInner() {
		remove();
	}

	void RenderBufferToken::TokenInner::remove() {
		if (id != 0) {
			glDeleteRenderbuffers(1, &id);
			id = 0;
		}
	}

	RenderBufferToken::RenderBufferToken() {
		unsigned int id = 0;
		glGenRenderbuffers(1, &id);

		token = std::make_shared<TokenInner>(id);
	}


	RenderBufferToken& RenderBufferToken::operator= (const RenderBufferToken& other) {
		if (this != &other) {
			token->remove();
			token = other.token;
		}

		return *this;
	}



	FrameBufferToken::~FrameBufferToken() {
		if (token != 0)
			glDeleteFramebuffers(1, &token);
	}

}