#ifndef VOLUMETRIC_H
#define VOLUMETRIC_H

#include "postprocessing.h"
#include "../utility/worldinformation.h"

namespace geeL {

	class RenderScene;
	class SpotLight;

	class VolumetricLight : public PostProcessingEffect, public WorldMapRequester, public CameraRequester {

	public:
		VolumetricLight(const SpotLight& light, 
			float density = 1.f, float minDistance = 1.f, unsigned int samples = 30);

		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		unsigned int getSampleCount() const;
		void setSampleCount(unsigned int samples);

		float getDensity() const;
		void setDensity(float density);

		float getMinDistance() const;
		void setMinDistance(float distance);

	protected:
		virtual void bindValues();

	private:
		unsigned int samples;
		float density;
		float minDistance;
		const SpotLight& light;

		ShaderLocation projectionLocation;
		ShaderLocation invViewLocation;

	};
}

#endif
