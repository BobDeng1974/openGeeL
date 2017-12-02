#ifndef TEXTUREPROVIDER_H
#define TEXTUREPROVIDER_H

#include <functional>
#include <map>
#include <list>
#include "texturetype.h"


namespace geeL {

	enum class ResolutionPreset;
	class GBuffer;
	class RenderTarget;
	class RenderWindow;
	class TextureParameters;
	class TextureTarget;
	class TextureWrapper;


	//Interface for objects that can manage texture memory automatically
	class ITextureProvider {

	public:
		virtual const RenderTarget& requestAlbedo() const = 0;
		virtual const RenderTarget& requestPositionRoughness() const = 0;
		virtual const RenderTarget& requestNormalMetallic() const = 0;
		virtual const RenderTarget* requestEmissivity() const = 0;
		virtual const RenderTarget* requestOcclusion() const = 0;
		virtual RenderTarget* requestOcclusion() = 0;

		//Request texture with default properties (Properties of final screen texture)
		virtual TextureTarget& requestDefaultTexture() = 0;

		//Returns the current iteration of the final renderered image for 
		//rendering purposes. If requester updates said image, the method 
		//'updateCurrentImage' should be called afterwards
		virtual TextureTarget& requestCurrentImage() = 0;
		virtual TextureTarget& requestCurrentSpecular() = 0;

		//Set current iteration of final rendererd image to given one.
		//Important: Current image texture will be reclaimed automatically
		//and doesn't need to be returned
		virtual void updateCurrentImage(TextureTarget& texture) = 0;
		virtual void updateCurrentSpecular(TextureTarget& texture) = 0;

		virtual TextureWrapper requestTexture(ResolutionPreset resolution,
			ColorType colorType = ColorType::RGB,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat,
			AnisotropicFilter aFilter = AnisotropicFilter::None) = 0;

		virtual TextureTarget& requestTextureManual(ResolutionPreset resolution,
			ColorType colorType = ColorType::RGB,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat,
			AnisotropicFilter aFilter = AnisotropicFilter::None) = 0;

		virtual void returnTexture(TextureTarget& texture) = 0;

	};



	class TextureProvider : public ITextureProvider {

	public:
		TextureProvider(const RenderWindow& window, GBuffer& gBuffer);
		~TextureProvider();

		virtual const RenderTarget& requestAlbedo() const;
		virtual const RenderTarget& requestPositionRoughness() const;
		virtual const RenderTarget& requestNormalMetallic() const;
		virtual const RenderTarget* requestEmissivity() const;
		virtual const RenderTarget* requestOcclusion() const;
		virtual RenderTarget* requestOcclusion();

		virtual TextureTarget& requestDefaultTexture();
		virtual TextureTarget& requestCurrentImage();
		virtual TextureTarget& requestCurrentSpecular();

		virtual void updateCurrentImage(TextureTarget& texture);
		virtual void updateCurrentSpecular(TextureTarget& texture);

		virtual TextureWrapper requestTexture(ResolutionPreset resolution, ColorType colorType,
			FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter);

		virtual TextureTarget& requestTextureManual(ResolutionPreset resolution, ColorType colorType,
			FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter);

		virtual void returnTexture(TextureTarget& texture);


		void cleanupCache();

	private:
		class MonitoredList {

		public:
			TextureTarget* pop();
			void push(TextureTarget* texture);
			void flush();

			bool isEmpty() const;
			unsigned int accessCount() const;
			unsigned int elementCount() const;

			std::list<TextureTarget*>::iterator begin();
			std::list<TextureTarget*>::iterator end();

		private:
			unsigned int accesses = 0;
			std::list<TextureTarget*> list;

		};

		const unsigned int cacheClearingRate = 10; //Cleanup only every n'th frame
		unsigned int currentRate = 0;

		const RenderWindow& window;
		GBuffer& gBuffer;
		TextureTarget* diffuse;
		TextureTarget* specular;
		std::function<void(TextureTarget&)> callback;

		std::map<ResolutionScale, std::map<ColorType, MonitoredList>> textures;
		std::map<FilterMode, std::map<WrapMode, std::map<AnisotropicFilter, TextureParameters>>> parameters;


		TextureParameters& getParameters(FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter);

	};



	class TextureWrapper {

	public:
		TextureWrapper(TextureTarget& texture, std::function<void(TextureTarget&)>& onDestroy);
		TextureWrapper(TextureWrapper&& other);
		~TextureWrapper();

		TextureTarget& getTexture();

	private:
		TextureTarget* texture;
		std::function<void(TextureTarget&)>& onDestroy;

		TextureWrapper() = delete;
		TextureWrapper(const TextureWrapper& other) = delete;
		TextureWrapper& operator=(const TextureWrapper& other) = delete;
		TextureWrapper& operator=(TextureWrapper&& other) = delete;

	};

}

#endif

