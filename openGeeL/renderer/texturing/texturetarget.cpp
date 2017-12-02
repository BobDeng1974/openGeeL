#include <iostream>
#include "texturetarget.h"

namespace geeL {

	TextureTarget::TextureTarget(std::unique_ptr<Texture> texture)
		: RenderTarget(std::move(texture)) {}


	void TextureTarget::setRenderResolution() const {
		getTexture().setRenderResolution();
	}

	Resolution TextureTarget::getRenderResolution() const {
		return getTexture().getScreenResolution();
	}

	unsigned int TextureTarget::getSize() const {
		return 1;
	}

	void TextureTarget::attachParameters(const TextureParameters& parameters) {
		getTexture().attachParameters(parameters);
	}

	void TextureTarget::detachParameters() {
		getTexture().detachParameters();
	}

	void TextureTarget::assign(unsigned int position) const {
		getTexture().assign(position);
	}

}