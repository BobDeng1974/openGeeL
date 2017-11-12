#ifndef TEXTUREPROVIDER_H
#define TEXTUREPROVIDER_H

#include <functional>
#include <map>
#include <list>
#include "texturetype.h"


namespace geeL {

	enum class ResolutionPreset;
	class RenderTexture;
	class RenderWindow;
	class TextureParameters;
	class TextureWrapper;


	//Interface for objects that can manage texture memory automatically
	class ITextureProvider {

	public:
		virtual TextureWrapper requestTexture(ResolutionPreset resolution, ColorType colorType,
			const TextureParameters& parameters) = 0;

	};



	class TextureProvider : public ITextureProvider {

	public:
		TextureProvider(const RenderWindow& window);
		~TextureProvider();

		virtual TextureWrapper requestTexture(ResolutionPreset resolution, ColorType colorType,
			const TextureParameters& parameters);


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
		std::function<void(RenderTexture&, ResolutionPreset)> callback;
		std::map<ResolutionPreset, std::map<ColorType, MonitoredList>> textures;


		void textureCallback(RenderTexture& texture, ResolutionPreset resolution);

	};



	class TextureWrapper {

	public:
		TextureWrapper(RenderTexture& texture,
			ResolutionPreset preset,
			std::function<void(RenderTexture&,
				ResolutionPreset)>& onDestroy);

		TextureWrapper(TextureWrapper&& other);
		~TextureWrapper();

		RenderTexture& getTexture();

	private:
		RenderTexture* texture;
		ResolutionPreset preset;
		std::function<void(RenderTexture&, ResolutionPreset)>& onDestroy;

		TextureWrapper() = delete;
		TextureWrapper(const TextureWrapper& other) = delete;
		TextureWrapper& operator=(const TextureWrapper& other) = delete;
		TextureWrapper& operator=(TextureWrapper&& other) = delete;

	};

}

#endif

