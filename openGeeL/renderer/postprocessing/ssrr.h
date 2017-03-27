#ifndef SSRR_H
#define SSRR_H

#include "postprocessing.h"
#include "../utility/worldinformation.h"

namespace geeL {

	//Screen Space Raycasted Reflections post effect
	class SSRR : public PostProcessingEffect, public WorldInformationRequester {

	public:
		SSRR();

		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

		virtual void addWorldInformation(std::map<WorldMaps, unsigned int> maps,
			std::map<WorldMatrices, const glm::mat4*> matrices,
			std::map<WorldVectors, const glm::vec3*> vectors);

	protected:
		virtual void bindValues();

	private:
		const glm::mat4x4* projectionMatrix;
		ShaderLocation projectionLocation;

	};
}

#endif

