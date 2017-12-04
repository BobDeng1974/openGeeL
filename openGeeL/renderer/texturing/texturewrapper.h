#ifndef TEXTUREWRAPPER_H
#define TEXTUREWRAPPER_H

#include <functional>

namespace geeL {

	class RenderTexture;


	class TextureWrapper {

	public:
		TextureWrapper(RenderTexture& texture, std::function<void(RenderTexture&)>& onDestroy);
		TextureWrapper(TextureWrapper&& other);
		~TextureWrapper();

		RenderTexture& getTexture();

	private:
		RenderTexture* texture;
		std::function<void(RenderTexture&)>& onDestroy;

		TextureWrapper() = delete;
		TextureWrapper(const TextureWrapper& other) = delete;
		TextureWrapper& operator=(const TextureWrapper& other) = delete;
		TextureWrapper& operator=(TextureWrapper&& other) = delete;

	};


	inline TextureWrapper::TextureWrapper(RenderTexture& texture,
		std::function<void(RenderTexture&)>& onDestroy)
		: texture(&texture)
		, onDestroy(onDestroy) {}


	inline TextureWrapper::TextureWrapper(TextureWrapper&& other) : texture(other.texture),
		onDestroy(other.onDestroy) {

		other.texture = nullptr;
	}

	inline TextureWrapper::~TextureWrapper() {
		if (texture != nullptr)
			onDestroy(*texture);
	}

	inline RenderTexture& TextureWrapper::getTexture() {
		assert(texture != nullptr);

		return *texture;
	}

}

#endif
