#ifndef DEFAULTPOST_H
#define DEFAULTPOST_H

#include "postprocessing.h"

namespace geeL {

	class ImageTexture;

	class DefaultPostProcess : public PostProcessingEffect {

	public:
		DefaultPostProcess(float exposure = 1.f);
		DefaultPostProcess(const DefaultPostProcess& other);
		~DefaultPostProcess();

		DefaultPostProcess& operator= (const DefaultPostProcess& other);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		float getExposure() const;
		void setExposure(float exposure);


	protected:
		float exposure;
		ImageTexture* noise;
		ShaderLocation exposureLocation;

	};
}

#endif

