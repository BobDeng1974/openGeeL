#ifndef FRAMEBUFFERTOKEN_H
#define FRAMEBUFFERTOKEN_H

namespace geeL {

	//Wrapper for framebuffer id that manages GPU memory automatically
	class FrameBufferToken {

	public:
		unsigned int token;

		FrameBufferToken();
		~FrameBufferToken();

		bool operator== (unsigned int other) const;
		bool operator!= (unsigned int other) const;

	};


	inline FrameBufferToken::FrameBufferToken() : token(0) {}

	inline bool FrameBufferToken::operator== (unsigned int other) const {
		return token == other;
	}

	inline bool FrameBufferToken::operator!= (unsigned int other) const {
		return token != other;
	}

}

#endif
