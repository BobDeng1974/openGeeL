#include <iostream>
#include <cassert>
#include "framebuffer/gbuffer.h"
#include "utility/resolution.h"
#include "window.h"
#include "framebuffer/rendertarget.h"
#include "texturetarget.h"
#include "textureparams.h"
#include "textureprovider.h"

namespace geeL {

	TextureWrapper::TextureWrapper(TextureTarget& texture,
		std::function<void(TextureTarget&)>& onDestroy)
			: texture(&texture)
			, onDestroy(onDestroy) {}


	TextureWrapper::TextureWrapper(TextureWrapper&& other) : texture(other.texture), 
		onDestroy(other.onDestroy) {

		other.texture = nullptr;
	}

	TextureWrapper::~TextureWrapper() {
		if(texture != nullptr)
			onDestroy(*texture);
	}

	TextureTarget& TextureWrapper::getTexture() {
		assert(texture != nullptr);

		return *texture;
	}


	TextureProvider::TextureProvider(const RenderWindow& window, GBuffer& gBuffer) 
		: window(window)
		, gBuffer(gBuffer)
		, diffuse(nullptr)
		, specular(nullptr) {

		callback = [this](TextureTarget& texture) { returnTexture(texture); };

	}

	TextureProvider::~TextureProvider() {
		if (diffuse != nullptr) delete diffuse;
		if (specular != nullptr) delete specular;

		for (auto resolutionIt(textures.begin()); resolutionIt != textures.end(); resolutionIt++) {
			auto& colors = resolutionIt->second;

			for (auto colorsIt(colors.begin()); colorsIt != colors.end(); colorsIt++) {
				auto& tex = colorsIt->second;

				for (auto texturesIt(tex.begin()); texturesIt != tex.end(); texturesIt++) {
					RenderTarget* texture = *texturesIt;
					delete texture;
				}
			}
		}
	}

	const RenderTarget& TextureProvider::requestAlbedo() const {
		return gBuffer.getDiffuse();
	}

	const RenderTarget& TextureProvider::requestPositionRoughness() const {
		return gBuffer.getPositionRoughness();
	}

	const RenderTarget& TextureProvider::requestNormalMetallic() const {
		return gBuffer.getNormalMetallic();
	}

	const RenderTarget* TextureProvider::requestEmissivity() const {
		return gBuffer.getEmissivity();
	}

	const RenderTarget* TextureProvider::requestOcclusion() const {
		return gBuffer.getOcclusion();
	}

	RenderTarget* TextureProvider::requestOcclusion() {
		return &gBuffer.requestOcclusion();
	}

	TextureTarget& TextureProvider::requestDefaultTexture() {
		return requestTextureManual(ResolutionPreset::FULLSCREEN, ColorType::RGBA16, 
			FilterMode::None, WrapMode::ClampEdge, AnisotropicFilter::None);
	}

	TextureTarget& TextureProvider::requestCurrentImage() {
		if (diffuse == nullptr)
			diffuse = &requestDefaultTexture();

		return *diffuse;
	}

	TextureTarget& TextureProvider::requestCurrentSpecular() {
		if (specular == nullptr)
			specular = &requestDefaultTexture();

		return *specular;
	}

	void TextureProvider::updateCurrentImage(TextureTarget& texture) {
		if (diffuse != &texture) {
			//Assing diffuse to new texture and throw 
			//old texture back into texture pool
			if(diffuse != nullptr) returnTexture(*diffuse);
			diffuse = &texture;
		}
	}

	void TextureProvider::updateCurrentSpecular(TextureTarget& texture) {
		if (specular != &texture) {
			if (specular != nullptr) returnTexture(*specular);
			specular = &texture;
		}
	}


	TextureWrapper TextureProvider::requestTexture(ResolutionPreset resolution, ColorType colorType,
		FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter) {

		TextureTarget* texture = &requestTextureManual(resolution, colorType,
			filterMode, wrapMode, aFilter);

		return TextureWrapper(*texture, callback);
	}

	TextureTarget& TextureProvider::requestTextureManual(ResolutionPreset resolution, ColorType colorType,
		FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter) {

		auto resolutionIt(textures.find(resolution));
		if (resolutionIt != textures.end()) {
			auto& colors = resolutionIt->second;

			auto colorsIt(colors.find(colorType));
			if (colorsIt != colors.end()) {

				auto& tex = colorsIt->second;
				if (!tex.isEmpty()) {
					TextureTarget* texture = tex.pop();
					texture->attachParameters(getParameters(filterMode, wrapMode, aFilter));

					return *texture;
				}
			}
		}

		//RenderTarget* newTexture = new RenderTarget(Resolution(window.getResolution(), resolution), colorType);
		TextureTarget* newTexture = TextureTarget::createTextureTargetPtr<Texture2D>(
			Resolution(window.getResolution(), resolution), colorType).release();

		newTexture->attachParameters(getParameters(filterMode, wrapMode, aFilter));

		return *newTexture;
	}


	void TextureProvider::returnTexture(TextureTarget& texture) {
		const TextureTarget* target = static_cast<const TextureTarget*>(&texture);
		const Texture2D* t = dynamic_cast<const Texture2D*>(&target->getTexture());
		assert(t != nullptr);

		//TODO: maybe detach parameters here
		textures[t->getScale()][t->getColorType()].push(&texture);
	}

	void TextureProvider::cleanupCache() {
		currentRate = (1 + currentRate) % cacheClearingRate;
		if (currentRate != 0) return;

		for (auto resolutionIt(textures.begin()); resolutionIt != textures.end(); resolutionIt++) {
			auto& colors = resolutionIt->second;

			for (auto colorsIt(colors.begin()); colorsIt != colors.end(); colorsIt++) {
				auto& tex = colorsIt->second;

				unsigned int elCount = tex.elementCount();
				unsigned int acCount = tex.accessCount();

				tex.flush();

				if (acCount > elCount) continue;

				//TODO: Fine-tune this heuristic later
				unsigned int deleteHeuristic = tex.elementCount() - tex.accessCount();
				for (unsigned int i = 0; i < deleteHeuristic; i++) {
					RenderTarget* texture = tex.pop();
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

	

	TextureTarget* TextureProvider::MonitoredList::pop() {
		TextureTarget* texture = list.front();
		list.pop_front();
		accesses++;

		return texture;
	}


	void TextureProvider::MonitoredList::push(TextureTarget* texture) {
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

	std::list<TextureTarget*>::iterator TextureProvider::MonitoredList::begin() {
		return list.begin();
	}

	std::list<TextureTarget*>::iterator TextureProvider::MonitoredList::end() {
		return list.end();
	}

}