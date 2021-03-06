#ifndef DEFAULTPOST_H
#define DEFAULTPOST_H

#include "memory/memoryobject.h"
#include "postprocessing.h"

namespace geeL {

	class ImageTexture;
	class ITexture;


	enum class TonemappingMethod {
		SimpleReinhardt1 = 0,
		SimpleReinhardt2 = 1,
		HejlBurgessDawson = 2,
		Uncharted2 = 3,
		ACESFilm = 4

	};


	class DefaultPostProcess : public PostProcessingEffectFS {

	public:
		DefaultPostProcess(const Resolution& windowResolution, float exposure = 1.f, 
			TonemappingMethod method = TonemappingMethod::Uncharted2, bool adaptive = false);

		DefaultPostProcess(const DefaultPostProcess& other);
		DefaultPostProcess& operator= (const DefaultPostProcess& other);


		virtual void init(const PostProcessingParameter& parameter);
		virtual void draw();

		void setCustomImage(const ITexture* const texture = nullptr);

		float getExposure() const;
		void setExposure(float exposure);

		bool getAdaptiveExposure() const;
		void setAdaptiveExposure(bool value);

		virtual std::string toString() const;

	protected:
		del_unique_ptr<ImageTexture> noise;
		const ITexture* customTexture;

	private:
		bool adaptiveExposure;
		const Resolution& resolution;

	};


	inline std::string DefaultPostProcess::toString() const {
		return "Default";
	}

}

#endif

