#ifndef TEXTUREPARAMETERS_H
#define TEXTUREPARAMETERS_H

#include "texturetype.h"

namespace geeL {

	class TextureParameters {

	public:
		TextureParameters(FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::Repeat, 
			AnisotropicFilter aFilter = AnisotropicFilter::None);
		TextureParameters(TextureParameters&& other);
		~TextureParameters();

		TextureParameters& operator=(TextureParameters&& other);

		void bind(unsigned int layer) const;
		static void unbind(unsigned int layer);

	private:
		unsigned int id;
		FilterMode filterMode;
		WrapMode wrapMode;
		AnisotropicFilter aFilter;

		TextureParameters(const TextureParameters& other) = delete;
		TextureParameters& operator= (const TextureParameters& other) = delete;

		void initFilterMode(FilterMode mode);
		void initWrapMode(WrapMode mode);
		void initAnisotropyFilter(AnisotropicFilter filter);

		void remove();

	};

}

#endif
