#include <iostream>
#include <cassert>
#include "utility/resolution.h"
#include "texturewrapper.h"
#include "rendertexture.h"
#include "textureparams.h"
#include "textureprovider.h"

namespace geeL {

	TextureProvider::TextureProvider(const Resolution& resolution) 
		: resolution(resolution)
		, diffuse(nullptr)
		, specular(nullptr)
		, previousDiffuse(nullptr)
		, position(nullptr)
		, normal(nullptr)
		, albedo(nullptr)
		, properties(nullptr) {

		callback = [this](RenderTexture& texture) { returnTexture(texture); };
	}

	TextureProvider::~TextureProvider() {
		if (diffuse != nullptr) delete diffuse;
		if (specular != nullptr) delete specular;
		if (previousDiffuse != nullptr) delete previousDiffuse;
		if (position != nullptr) delete position;
		if (normal != nullptr) delete normal;
		if (albedo != nullptr) delete albedo;
		if (properties != nullptr) delete properties;

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


	RenderTexture& TextureProvider::requestAlbedo() {
		if (albedo == nullptr)
			albedo = &requestTextureManual(ResolutionPreset::FULLSCREEN, ColorType::RGBA,
				FilterMode::None, WrapMode::Repeat, AnisotropicFilter::None);

		return *albedo;
	}

	RenderTexture& TextureProvider::requestPosition() {
		if (position == nullptr)
			position = &requestTextureManual(ResolutionPreset::FULLSCREEN, ColorType::RGB16,
				FilterMode::None, WrapMode::Repeat, AnisotropicFilter::None);

		return *position;
	}

	RenderTexture& TextureProvider::requestNormal() {
		if (normal == nullptr)
			normal = &requestTextureManual(ResolutionPreset::FULLSCREEN, ColorType::RGBA16,
				FilterMode::None, WrapMode::Repeat, AnisotropicFilter::None);

		return *normal;
	}

	RenderTexture& TextureProvider::requestProperties() {
		if (properties == nullptr)
			properties = &requestTextureManual(ResolutionPreset::FULLSCREEN, ColorType::RGBA,
				FilterMode::None, WrapMode::Repeat, AnisotropicFilter::None);

		return *properties;
	}

	RenderTexture& TextureProvider::requestDefaultTexture() {
		return requestTextureManual(ResolutionPreset::FULLSCREEN, ColorType::RGBA16, 
			FilterMode::None, WrapMode::ClampEdge, AnisotropicFilter::None);
	}

	RenderTexture& TextureProvider::requestCurrentImage() {
		if (diffuse == nullptr)
			diffuse = &requestDefaultTexture();

		return *diffuse;
	}

	RenderTexture& TextureProvider::requestCurrentSpecular() {
		if (specular == nullptr)
			specular = &requestDefaultTexture();

		return *specular;
	}

	void TextureProvider::updateCurrentImage(RenderTexture& texture) {
		if (diffuse != &texture) {
			//Assign diffuse to new texture and throw 
			//old texture back into texture pool
			if(diffuse != nullptr) returnTexture(*diffuse);
			diffuse = &texture;
		}
	}

	void TextureProvider::updateCurrentSpecular(RenderTexture& texture) {
		if (specular != &texture) {
			if (specular != nullptr) returnTexture(*specular);
			specular = &texture;
		}
	}

	RenderTexture& TextureProvider::requestPreviousImage() const {
		if (previousDiffuse != nullptr)
			return *previousDiffuse;

		return *diffuse;
	}

	void TextureProvider::swap() {
		if (previousDiffuse != nullptr)
			returnTexture(*previousDiffuse);

		previousDiffuse = diffuse;
		diffuse = nullptr;
	}


	TextureWrapper TextureProvider::requestTexture(ResolutionPreset resolution, ColorType colorType,
		FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter) {

		RenderTexture* texture = &requestTextureManual(resolution, colorType, 
			filterMode, wrapMode, aFilter);

		return TextureWrapper(*texture, callback);
	}

	RenderTexture& TextureProvider::requestTextureManual(ResolutionPreset resolution, ColorType colorType, 
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

					return *texture;
				}
			}
		}

		RenderTexture* newTexture = new RenderTexture(Resolution(this->resolution, resolution), colorType);
		newTexture->attachParameters(getParameters(filterMode, wrapMode, aFilter));

		return *newTexture;
	}


	void TextureProvider::returnTexture(RenderTexture& texture) {
		//TODO: maybe detach parameters here
		textures[texture.getScale()][texture.getColorType()].push(&texture);
	}

	const Resolution& TextureProvider::getRenderResolution() const {
		return resolution;
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

	TextureParameters& TextureProvider::getDefaultParameters() {
		return getParameters(FilterMode::None, WrapMode::ClampEdge, AnisotropicFilter::None);
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

	ResolutionPreset ITextureProvider::getClosestPreset(const Resolution& customResolution) const {
		float scale = float(customResolution.getWidth()) / float(getRenderResolution().getWidth());
		return getRenderPreset(ResolutionScale(scale));
	}

}