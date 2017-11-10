#ifndef VOLUMETRIC_H
#define VOLUMETRIC_H

#include "postprocessing.h"
#include "additiveeffect.h"
#include "utility/worldinformation.h"

namespace geeL {

	class RenderScene;
	class SpotLight;

	class VolumetricLight : public AdditiveEffect, public WorldMapRequester, public CameraRequester {

	public:
		VolumetricLight(SpotLight& light,  float density = 1.f, float minDistance = 1.f, 
			unsigned int samples = 30, bool useCookie = false);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		unsigned int getSampleCount() const;
		void setSampleCount(unsigned int samples);

		float getDensity() const;
		void setDensity(float density);

		float getMinDistance() const;
		void setMinDistance(float distance);

		virtual std::string toString() const;

	private:
		unsigned int samples;
		bool useCookie;
		float density;
		float minDistance;
		SpotLight& light;

		ShaderLocation projectionLocation;
		ShaderLocation invViewLocation;

	};


	inline std::string VolumetricLight::toString() const {
		return "Volumetric light";
	}

}

#endif
