#ifndef TEXTUREPROVIDER_H
#define TEXTUREPROVIDER_H

#include <functional>
#include <map>
#include <list>
#include "utility/resolution.h"
#include "texturetype.h"

namespace geeL {

	enum class ResolutionPreset;
	class GBuffer;
	class RenderTexture;
	class RenderWindow;
	class Texture2D;
	class TextureCube;
	class TextureParameters;
	class TextureWrapper;


	//Interface for objects that can manage texture memory automatically
	class ITextureProvider {

	public:
		virtual RenderTexture& requestAlbedo() = 0;
		virtual RenderTexture& requestPosition() = 0;
		virtual RenderTexture& requestNormal() = 0;
		virtual RenderTexture& requestProperties() = 0;

		//Returns emission texture if emissivity is enabled, otherwise null
		virtual RenderTexture* const requestEmission() = 0;

		//Request texture with default properties (Properties of final screen texture)
		virtual RenderTexture& requestDefaultTexture() = 0;

		//Returns the current iteration of the final renderered image for 
		//rendering purposes. If requester updates said image, the method 
		//'updateCurrentImage' should be called afterwards
		virtual RenderTexture& requestCurrentImage() = 0;
		virtual RenderTexture& requestCurrentSpecular() = 0;

		//Set current iteration of final rendererd image to given one.
		//Important: Current image texture will be reclaimed automatically
		//and doesn't need to be returned
		virtual void updateCurrentImage(RenderTexture& texture) = 0;
		virtual void updateCurrentSpecular(RenderTexture& texture) = 0;


		//Returns a 1x1 dummy 2D texture
		virtual const Texture2D& requestDummy2D() = 0;

		//Returns a 6x1x1 dummy cubic texture
		virtual const TextureCube& requestDummyCube() = 0;


		virtual RenderTexture& requestPreviousImage() const = 0;
		//virtual RenderTexture& requestPreviousPosition() const = 0;

		virtual TextureWrapper requestTexture(ResolutionPreset resolution,
			ColorType colorType = ColorType::RGB,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat,
			AnisotropicFilter aFilter = AnisotropicFilter::None) = 0;

		virtual RenderTexture& requestTextureManual(ResolutionPreset resolution, 
			ColorType colorType = ColorType::RGB,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat,
			AnisotropicFilter aFilter = AnisotropicFilter::None) = 0;

		virtual void returnTexture(RenderTexture& texture) = 0;

		virtual TextureParameters& getParameters(FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter) = 0;
		virtual TextureParameters& getDefaultParameters() = 0;

		virtual const Resolution& getRenderResolution() const = 0;
		ResolutionPreset getClosestPreset(const Resolution& customResolution) const;

	};



	class TextureProvider : public ITextureProvider {

	public:
		TextureProvider(const Resolution& resolution);
		~TextureProvider();

		virtual RenderTexture& requestAlbedo();
		virtual RenderTexture& requestPosition();
		virtual RenderTexture& requestNormal();
		virtual RenderTexture& requestProperties();
		virtual RenderTexture* const requestEmission();

		virtual RenderTexture& requestDefaultTexture();
		virtual RenderTexture& requestCurrentImage();
		virtual RenderTexture& requestCurrentSpecular();

		virtual void updateCurrentImage(RenderTexture& texture);
		virtual void updateCurrentSpecular(RenderTexture& texture);

		virtual const Texture2D& requestDummy2D();
		virtual const TextureCube& requestDummyCube();

		virtual RenderTexture& requestPreviousImage() const;
		//virtual RenderTexture& requestPreviousPosition() const;

		virtual TextureWrapper requestTexture(ResolutionPreset resolution,
			ColorType colorType = ColorType::RGB,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat,
			AnisotropicFilter aFilter = AnisotropicFilter::None);

		virtual RenderTexture& requestTextureManual(ResolutionPreset resolution,
			ColorType colorType = ColorType::RGB,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat,
			AnisotropicFilter aFilter = AnisotropicFilter::None);

		virtual void returnTexture(RenderTexture& texture);

		virtual TextureParameters& getParameters(FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter);
		virtual TextureParameters& getDefaultParameters();

		virtual const Resolution& getRenderResolution() const;

		//Should be called at end of frame. Current images will be
		//converted to previous images
		void swap();
		void cleanupCache();

	private:
		class MonitoredList {

		public:
			RenderTexture* pop();
			void push(RenderTexture* texture);
			void flush();

			bool isEmpty() const;
			unsigned int accessCount() const;
			unsigned int elementCount() const;

			std::list<RenderTexture*>::iterator begin();
			std::list<RenderTexture*>::iterator end();

		private:
			unsigned int accesses = 0;
			std::list<RenderTexture*> list;

		};

		const unsigned int cacheClearingRate = 1; //Cleanup only every n'th frame
		unsigned int currentRate = 0;
		Resolution resolution;

		RenderTexture* diffuse;
		RenderTexture* specular;
		RenderTexture* previousDiffuse;
		RenderTexture* albedo;
		RenderTexture* position;
		RenderTexture* normal;
		RenderTexture* properties;
		RenderTexture* emission;

		Texture2D* dummy2D;
		TextureCube* dummyCube;

		std::function<void(RenderTexture&)> callback;

		std::map<ResolutionScale, std::map<ColorType, MonitoredList>> textures;
		std::map<FilterMode, std::map<WrapMode, std::map<AnisotropicFilter, TextureParameters>>> parameters;

	};

}

#endif

