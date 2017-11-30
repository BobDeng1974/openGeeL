#ifndef DEFAULTPOST_H
#define DEFAULTPOST_H

#include "postprocessing.h"

namespace geeL {

	class ImageTexture;
	class ITexture;

	class DefaultPostProcess : public PostProcessingEffectFS {

	public:
		DefaultPostProcess(float exposure = 1.f);
		DefaultPostProcess(const DefaultPostProcess& other);
		virtual ~DefaultPostProcess();

		DefaultPostProcess& operator= (const DefaultPostProcess& other);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void draw();

		void setCustomImage(const ITexture* const texture = nullptr);

		float getExposure() const;
		void setExposure(float exposure);

		virtual std::string toString() const;

	protected:
		ImageTexture* noise;
		const ITexture* customTexture;

	};


	inline std::string DefaultPostProcess::toString() const {
		return "Default";
	}

}

#endif

