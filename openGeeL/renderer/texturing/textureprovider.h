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
	class TextureParameters;
	class TextureWrapper;


	//Interface for objects that can manage texture memory automatically
	class ITextureProvider {

	public:
		virtual const RenderTexture& requestAlbedo() const = 0;
		virtual const RenderTexture& requestPosition() const = 0;
		virtual const RenderTexture& requestNormal() const = 0;
		virtual const RenderTexture& requesOcclusionEmissivityRoughnessMetallic() const = 0;
		virtual RenderTexture& requestOcclusion() const = 0;

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

	};



	class TextureProvider : public ITextureProvider {

	public:
		TextureProvider(const RenderWindow& window, GBuffer& gBuffer);
		~TextureProvider();

		virtual const RenderTexture& requestAlbedo() const;
		virtual const RenderTexture& requestPosition() const;
		virtual const RenderTexture& requestNormal() const;
		virtual const RenderTexture& requesOcclusionEmissivityRoughnessMetallic() const;
		virtual RenderTexture& requestOcclusion() const;

		virtual RenderTexture& requestDefaultTexture();
		virtual RenderTexture& requestCurrentImage();
		virtual RenderTexture& requestCurrentSpecular();

		virtual void updateCurrentImage(RenderTexture& texture);
		virtual void updateCurrentSpecular(RenderTexture& texture);

		virtual TextureWrapper requestTexture(ResolutionPreset resolution, ColorType colorType,
			FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter);

		virtual RenderTexture& requestTextureManual(ResolutionPreset resolution, ColorType colorType,
			FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter);

		virtual void returnTexture(RenderTexture& texture);

		Resolution getRenderResolution() const;

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

		const unsigned int cacheClearingRate = 10; //Cleanup only every n'th frame
		unsigned int currentRate = 0;

		const RenderWindow& window;
		GBuffer& gBuffer;
		RenderTexture* diffuse;
		RenderTexture* specular;
		std::function<void(RenderTexture&)> callback;

		std::map<ResolutionScale, std::map<ColorType, MonitoredList>> textures;
		std::map<FilterMode, std::map<WrapMode, std::map<AnisotropicFilter, TextureParameters>>> parameters;


		TextureParameters& getParameters(FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter);

	};

}

#endif

