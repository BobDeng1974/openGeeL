#include <iostream>
#include "framebuffer/framebuffer.h"
#include "utility/glguards.h"
#include "utility/clearing.h"
#include "additiveeffect.h"

namespace geeL {

	AdditiveEffect::AdditiveEffect(const std::string& fragmentPath) 
		: PostProcessingEffectFS(fragmentPath) {}

	AdditiveEffect::AdditiveEffect(const std::string& vertexPath, const std::string& fragmentPath) 
		: PostProcessingEffectFS(vertexPath, fragmentPath) {}


	void AdditiveEffect::setImage(const Texture& texture) {
		std::cout << "Can't set main image buffer for additive effects\n";
	}

	void AdditiveEffect::draw() {
		std::cout << "Additive effect should be drawn with 'fill' method instead\n";

		PostProcessingEffectFS::draw();
	}

	void AdditiveEffect::fill() {
		BlendGuard blend;
		blend.blendAdd();

		if (parentBuffer != nullptr) {
			parentBuffer->fill([this]() {
				this->bindValues();
				this->bindToScreen();
			}, clearNothing);
		}
			
	}

}