#ifndef VOLUMETRIC_H
#define VOLUMETRIC_H

#include "postprocessing.h"
#include "../utility/worldinformation.h"

namespace geeL {

	class RenderScene;
	class SpotLight;

	class VolumetricLight : public PostProcessingEffectFS, public WorldMapRequester, public CameraRequester {

	public:
		VolumetricLight(const SpotLight& light, 
			float density = 1.f, float minDistance = 1.f, unsigned int samples = 30, bool useCookie = false);

		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		unsigned int getSampleCount() const;
		void setSampleCount(unsigned int samples);

		float getDensity() const;
		void setDensity(float density);

		float getMinDistance() const;
		void setMinDistance(float distance);

		virtual std::string toString() const;

	protected:
		virtual void bindValues();

	private:
		unsigned int samples;
		bool useCookie;
		float density;
		float minDistance;
		const SpotLight& light;

		ShaderLocation projectionLocation;
		ShaderLocation invViewLocation;

	};


	inline std::string VolumetricLight::toString() const {
		return "Volumetric light";
	}

}

#endif
