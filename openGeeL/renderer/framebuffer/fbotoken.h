#ifndef FRAMEBUFFERTOKEN_H
#define FRAMEBUFFERTOKEN_H

#include <memory>

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


	//Wrapper for RBO id that manages GPU memory automatically
	class RenderBufferToken {

	public:
		RenderBufferToken();
		RenderBufferToken(const RenderBufferToken& other);

		RenderBufferToken& operator= (const RenderBufferToken& other);

		bool operator== (const RenderBufferToken& other) const;
		bool operator!= (const RenderBufferToken& other) const;

		bool operator== (unsigned int other) const;
		bool operator!= (unsigned int other) const;

		operator unsigned int() const;

	private:
		struct TokenInner {

		public:
			unsigned int id;

			void remove();

			TokenInner(unsigned int id);
			~TokenInner();
		};

		std::shared_ptr<TokenInner> token;

	};



	inline FrameBufferToken::FrameBufferToken() : token(0) {}

	inline bool FrameBufferToken::operator== (unsigned int other) const {
		return token == other;
	}

	inline bool FrameBufferToken::operator!= (unsigned int other) const {
		return token != other;
	}


	inline RenderBufferToken::TokenInner::TokenInner(unsigned int id) : id(id) {}

	inline RenderBufferToken::RenderBufferToken(const RenderBufferToken& other) : token(other.token) {}


	inline bool RenderBufferToken::operator== (const RenderBufferToken& other) const {
		return token.get()->id == other.token.get()->id;
	}

	inline bool RenderBufferToken::operator!= (const RenderBufferToken& other) const {
		return token.get()->id != other.token.get()->id;
	}

	inline bool RenderBufferToken::operator== (unsigned int other) const {
		return token.get()->id == other;
	}
	inline bool RenderBufferToken::operator!= (unsigned int other) const {
		return token.get()->id != other;
	}

	inline RenderBufferToken::operator unsigned int() const {
		return token.get()->id;
	}

	
}

#endif
