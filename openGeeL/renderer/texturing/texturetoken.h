#ifndef TEXTURETOKEN_H
#define TEXTURETOKEN_H

#include <cassert>
#include <memory>

namespace geeL {

	//Wrapper for texture id that manages GPU memory automatically
	class TextureToken {

	public:
		TextureToken();
		TextureToken(const TextureToken& other);
		TextureToken(TextureToken&& other);

		TextureToken& operator= (const TextureToken& other);
		TextureToken& operator= (TextureToken&& other);

		bool operator== (const TextureToken& other) const;
		bool operator!= (const TextureToken& other) const;

		bool operator== (unsigned int other) const;
		bool operator!= (unsigned int other) const;

		operator unsigned int() const;

	private:
		struct TokenInner {

		public:
			unsigned int id;

			TokenInner(unsigned int id);
			~TokenInner();
		};

		std::shared_ptr<TokenInner> token;

	};


	inline bool TextureToken::operator== (const TextureToken& other) const {
		return token.get()->id == other.token.get()->id;
	}

	inline bool TextureToken::operator!= (const TextureToken& other) const {
		return token.get()->id != other.token.get()->id;
	}

	inline bool TextureToken::operator== (unsigned int other) const {
		return token.get()->id == other;
	}
	inline bool TextureToken::operator!= (unsigned int other) const {
		return token.get()->id != other;
	}

	inline TextureToken::operator unsigned int() const {
		return token.get()->id;
	}


}

#endif
