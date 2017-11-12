#include <iostream>
#include <cassert>
#include "framebuffer/gbuffer.h"
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




	TextureProvider::TextureProvider(const RenderWindow& window, GBuffer& gBuffer) 
		: window(window)
		, gBuffer(gBuffer) {

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

	const RenderTexture& TextureProvider::requestDiffuse() const {
		return gBuffer.getDiffuse();
	}

	const RenderTexture& TextureProvider::requestPositionRoughness() const {
		return gBuffer.getPositionRoughness();
	}

	const RenderTexture& TextureProvider::requestNormalMetallic() const {
		return gBuffer.getNormalMetallic();
	}

	const RenderTexture* TextureProvider::requestEmissivity() const {
		return gBuffer.getEmissivity();
	}

	const RenderTexture* TextureProvider::requestOcclusion() const {
		return gBuffer.getOcclusion();
	}


	TextureWrapper TextureProvider::requestTexture(ResolutionPreset resolution, ColorType colorType,
		FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter) {

		auto resolutionIt(textures.find(resolution));
		if (resolutionIt != textures.end()) {
			auto& colors = resolutionIt->second;

			auto colorsIt(colors.find(colorType));
			if (colorsIt != colors.end()) {

				auto& tex = colorsIt->second;
				if (!tex.isEmpty()) {
					RenderTexture* texture = tex.pop();
					texture->attachParameters(getParameters(filterMode, wrapMode, aFilter));


					return TextureWrapper(*texture, resolution, callback);
				}
			}
		}

		RenderTexture* newTexture = new RenderTexture(Resolution(window.getResolution(), resolution), colorType);
		newTexture->attachParameters(getParameters(filterMode, wrapMode, aFilter));

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
				unsigned int deleteHeuristic = fmax(0, 
					tex.elementCount() - tex.accessCount());

				for (unsigned int i = 0; i < deleteHeuristic; i++) {
					RenderTexture* texture = tex.pop();
					delete texture;
				}
			}
		}
	}

	
	TextureParameters& TextureProvider::getParameters(FilterMode filterMode, 
		WrapMode wrapMode, AnisotropicFilter aFilter) {

		auto filterIt(parameters.find(filterMode));
		if (filterIt != parameters.end()) {
			auto& filterObjects = filterIt->second;

			auto wrapIt(filterObjects.find(wrapMode));
			if (wrapIt != filterObjects.end()) {
				auto& wrapObjects = wrapIt->second;

				auto anIt(wrapObjects.find(aFilter));
				if (anIt != wrapObjects.end())
					return anIt->second;
			}
		}

		parameters[filterMode][wrapMode][aFilter] = TextureParameters(filterMode, wrapMode, aFilter);
		return parameters[filterMode][wrapMode][aFilter];
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