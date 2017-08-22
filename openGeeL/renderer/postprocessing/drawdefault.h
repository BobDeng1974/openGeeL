#ifndef DEFAULTPOST_H
#define DEFAULTPOST_H

#include "postprocessing.h"

namespace geeL {

	class ImageTexture;

	class DefaultPostProcess : public PostProcessingEffectFS {

	public:
		DefaultPostProcess(float exposure = 1.f);
		DefaultPostProcess(const DefaultPostProcess& other);
		virtual ~DefaultPostProcess();

		DefaultPostProcess& operator= (const DefaultPostProcess& other);

		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);

		float getExposure() const;
		void setExposure(float exposure);

		virtual std::string toString() const;

	protected:
		ImageTexture* noise;

	};


	inline std::string DefaultPostProcess::toString() const {
		return "Default";
	}

}

#endif

