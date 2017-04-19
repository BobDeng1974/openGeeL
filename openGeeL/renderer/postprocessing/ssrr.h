#ifndef SSRR_H
#define SSRR_H

#include "postprocessing.h"
#include "../utility/worldinformation.h"

namespace geeL {

	//Screen Space Raycasted Reflections post effect
	class SSRR : public PostProcessingEffect, public WorldMapRequester, public CameraRequester {

	public:
		SSRR();

		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	protected:
		virtual void bindValues();

	private:
		ShaderLocation projectionLocation;

	};
}

#endif

