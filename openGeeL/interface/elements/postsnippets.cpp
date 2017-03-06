#include "../guiwrapper.h"
#include "../../renderer/postprocessing/drawdefault.h"
#include "../../renderer/postprocessing/blurredeffect.h"
#include "../../renderer/postprocessing/colorcorrection.h"
#include "../../renderer/postprocessing/bloom.h"
#include "../../renderer/postprocessing/dof.h"
#include "../../renderer/postprocessing/godray.h"
#include "../../renderer/postprocessing/ssao.h"
#include "../../renderer/postprocessing/volumetriclight.h"
#include "postsnippets.h"
#include <iostream>

namespace geeL {

	DefaultSnippet::DefaultSnippet(DefaultPostProcess& def) : def(def) {}

	BlurredEffectSnippet::BlurredEffectSnippet(BlurredPostEffect& effect, GUISnippet& effectSnippet) 
		: effect(effect), effectSnippet(effectSnippet) {}

	ColorCorrectionSnippet::ColorCorrectionSnippet(ColorCorrection& color) : color(color) {}

	BloomSnippet::BloomSnippet(Bloom& bloom) : bloom(bloom) {}

	DepthOfFieldBlurredSnippet::DepthOfFieldBlurredSnippet(DepthOfFieldBlurred& dof) : dof(dof) {}

	GodRaySnippet::GodRaySnippet(GodRay& ray) : ray(ray) {}

	VolumetricLightSnippet::VolumetricLightSnippet(VolumetricLight& light) : light(light) {}

	SSAOSnippet::SSAOSnippet(SSAO& ssao) : ssao(ssao) {}


	void DefaultSnippet::draw(GUIContext* context) {
		float exposure = GUISnippets::drawBarFloat(context, def.getExposure(), 0.f, 50.f, 0.1f, "Exposure");
		def.setExposure(exposure);
	}

	std::string DefaultSnippet::toString() const {
		return "Default";
	}


	void BlurredEffectSnippet::draw(GUIContext* context) {
		
		float oldResolution = effect.getEffectResolution();
		float effectResolution = GUISnippets::drawBarFloat(context, 
			oldResolution, 0.f, 1.f, 0.001f, "Resolution");

		if (effectResolution != oldResolution)
			effect.resizeEffectResolution(effectResolution);

		effectSnippet.draw(context);
	}

	std::string BlurredEffectSnippet::toString() const {
		return "Blurred " + effectSnippet.toString();
	}


	void ColorCorrectionSnippet::draw(GUIContext* context) {
		color.r = GUISnippets::drawBarFloat(context, color.r, 0.f, 1.f, 0.001f, "Red");
		color.g = GUISnippets::drawBarFloat(context, color.g, 0.f, 1.f, 0.001f, "Green");
		color.b = GUISnippets::drawBarFloat(context, color.b, 0.f, 1.f, 0.001f, "Blue");
		color.h = GUISnippets::drawBarFloat(context, color.h, 0.f, 1.f, 0.001f, "Hue");
		color.s = GUISnippets::drawBarFloat(context, color.s, 0.f, 1.f, 0.001f, "Saturation");
		color.v = GUISnippets::drawBarFloat(context, color.v, 0.f, 1.f, 0.001f, "Brightness");
	}

	std::string ColorCorrectionSnippet::toString() const {
		return "Color Correction";
	}


	void BloomSnippet::draw(GUIContext* context) {
		float oldResolution = bloom.getEffectResolution();
		float effectResolution = GUISnippets::drawBarFloat(context,
			oldResolution, 0.f, 1.f, 0.001f, "Resolution");

		if (effectResolution != oldResolution)
			bloom.resizeEffectResolution(effectResolution);

		float scatter = GUISnippets::drawBarFloat(context, bloom.getScatter(), 0.f, 1.f, 0.005f, "Scatter");
		bloom.setScatter(scatter);
	}

	std::string BloomSnippet::toString() const {
		return "Bloom";
	}
	

	void DepthOfFieldBlurredSnippet::draw(GUIContext* context) {
		float oldResolution = dof.getBlurResolution();
		float blurResolution = GUISnippets::drawBarFloat(context,
			oldResolution, 0.f, 1.f, 0.001f, "Resolution");

		if (blurResolution != oldResolution)
			dof.resizeBlurResolution(blurResolution);

		float aperture = GUISnippets::drawBarFloat(context, dof.getAperture(), 0.f, 100.f, 0.1f, "Aperture");
		dof.setAperture(aperture);
	}

	std::string DepthOfFieldBlurredSnippet::toString() const {
		return "Depth of Field";
	}


	void GodRaySnippet::draw(GUIContext* context) {
		glm::vec3 position = GUISnippets::drawVector2(context, ray.getLightPosition(), "", 100, 0.1f);
		ray.setLightPosition(position);

		int samples = GUISnippets::drawBarInteger(context, ray.getSampleCount(), 0, 50, 1, "Samples");
		ray.setSampleCount(samples);
	}

	std::string GodRaySnippet::toString() const {
		return "God Ray";
	}
	

	void VolumetricLightSnippet::draw(GUIContext* context) {
		int samples = GUISnippets::drawBarInteger(context, light.getSampleCount(), 0, 500, 1, "Samples");
		light.setSampleCount(samples);

		float density = GUISnippets::drawBarFloat(context, light.getDensity(), 0.f, 5.f, 0.001f, "Density");
		light.setDensity(density);

		float distance = GUISnippets::drawBarFloat(context, light.getMinDistance(), 0.f, 50.f, 0.1f, "Distance");
		light.setMinDistance(distance);
	}
	
	std::string VolumetricLightSnippet::toString() const {
		return "Volumetric Light";
	}

	void SSAOSnippet::draw(GUIContext* context) {
		float radius = GUISnippets::drawBarFloat(context, ssao.getRadius(), 0.f, 100.f, 0.1f, "Radius");
		ssao.setRadius(radius);
	}

	std::string SSAOSnippet::toString() const {
		return "SSAO";
	}


}