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


	void AdditiveEffect::setImage(const Texture& texture) {}


	void AdditiveEffect::fill() {
		BlendGuard blend;
		blend.blendAdd();

		if (parentBuffer != nullptr) {
			parentBuffer->fill([this]() {
				if(active) PostProcessingEffectFS::draw();
			}, clearNothing);
		}
			
	}


}