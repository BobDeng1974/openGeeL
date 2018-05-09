#include <iostream>
#include "texturetarget.h"

namespace geeL {

	TextureTarget::TextureTarget(std::unique_ptr<Texture> texture)
		: FunctionalTexture(std::move(texture)) {}


	void TextureTarget::applyRenderResolution() const {
		getTexture().applyRenderResolution();
	}

	Resolution TextureTarget::getRenderPreset() const {
		return getTexture().getScreenResolution();
	}

	unsigned int TextureTarget::getSize() const {
		return 1;
	}

	void TextureTarget::assign(unsigned int position) const {
		getTexture().assign(position);
	}

}