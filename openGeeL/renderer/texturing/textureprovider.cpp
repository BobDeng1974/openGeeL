#include <iostream>
#include <cassert>
#include "utility/resolution.h"
#include "window.h"
#include "rendertexture.h"
#include "textureparams.h"
#include "textureprovider.h"

namespace geeL {

	TextureWrapper::TextureWrapper(
		RenderTexture& texture, 
		ResolutionPreset preset, 
		std::function<void(RenderTexture&, ResolutionPreset)>& onDestroy) 
			: texture(&texture)
			, preset(preset)
			, onDestroy(onDestroy) {}

	TextureWrapper::TextureWrapper(TextureWrapper&& other) : texture(other.texture), 
		preset(other.preset), onDestroy(other.onDestroy) {

		other.texture = nullptr;
	}

	TextureWrapper::~TextureWrapper() {
		if(texture != nullptr)
			onDestroy(*texture, preset);
	}

	RenderTexture& TextureWrapper::getTexture() {
		assert(texture != nullptr);

		return *texture;
	}




	TextureProvider::TextureProvider(const RenderWindow& window) : window(window) {
		callback = [this](RenderTexture& texture, ResolutionPreset preset) { 
			textureCallback(texture, preset); };

	}

	TextureProvider::~TextureProvider() {
		for (auto resolutionIt(textures.begin()); resolutionIt != textures.end(); resolutionIt++) {
			auto& colors = resolutionIt->second;

			for (auto colorsIt(colors.begin()); colorsIt != colors.end(); colorsIt++) {
				auto& tex = colorsIt->second;

				for (auto texturesIt(tex.begin()); texturesIt != tex.end(); texturesIt++) {
					RenderTexture* texture = *texturesIt;
					delete texture;
				}
			}
		}

	}

	TextureWrapper TextureProvider::requestTexture(ResolutionPreset resolution, ColorType colorType,
		const TextureParameters& parameters) {

		auto resolutionIt(textures.find(resolution));
		if (resolutionIt != textures.end()) {
			auto& colors = resolutionIt->second;

			auto colorsIt(colors.find(colorType));
			if (colorsIt != colors.end()) {

				auto& tex = colorsIt->second;
				if (!tex.isEmpty()) {
					RenderTexture* texture = tex.pop();
					texture->attachParameters(parameters);


					return TextureWrapper(*texture, resolution, callback);
				}
			}
		}

		RenderTexture* newTexture = new RenderTexture(Resolution(window.getResolution(), resolution), colorType);
		newTexture->attachParameters(parameters);

		return TextureWrapper(*newTexture, resolution, callback);
	}

	void TextureProvider::cleanupCache() {
		currentRate = (1 + currentRate) % cacheClearingRate;
		if (currentRate != 0) return;

		for (auto resolutionIt(textures.begin()); resolutionIt != textures.end(); resolutionIt++) {
			auto& colors = resolutionIt->second;

			for (auto colorsIt(colors.begin()); colorsIt != colors.end(); colorsIt++) {
				auto& tex = colorsIt->second;

				//TODO: Fine-tune this heuristic later
				unsigned int deleteHeuristic = fmaxf(0.f, 
					tex.elementCount() - tex.accessCount());

				for (unsigned int i = 0; i < deleteHeuristic; i++) {
					RenderTexture* texture = tex.pop();
					delete texture;
				}
			}
		}
	}

	void TextureProvider::textureCallback(RenderTexture& texture, ResolutionPreset resolution) {
		//TODO: maybe detach parameters here
		textures[resolution][texture.getColorType()].push(&texture);
	}

	

	RenderTexture* TextureProvider::MonitoredList::pop() {
		RenderTexture* texture = list.front();
		list.pop_front();
		accesses++;

		return texture;
	}


	void TextureProvider::MonitoredList::push(RenderTexture* texture) {
		list.push_back(texture);
	}

	void TextureProvider::MonitoredList::flush() {
		accesses = 0;
	}

	bool TextureProvider::MonitoredList::isEmpty() const {
		return list.size() == 0;
	}

	unsigned int TextureProvider::MonitoredList::accessCount() const {
		return accesses;
	}

	unsigned int TextureProvider::MonitoredList::elementCount() const {
		return list.size();
	}

	std::list<RenderTexture*>::iterator TextureProvider::MonitoredList::begin() {
		return list.begin();
	}

	std::list<RenderTexture*>::iterator TextureProvider::MonitoredList::end() {
		return list.end();
	}

}