#ifndef POSTEFFECTSNIPPETS_H
#define POSTEFFECTSNIPPETS_H

#include "guisnippets.h"

namespace geeL {

	class Bloom;
	class BlurredPostEffect;
	class ColorCorrection;
	class DefaultPostProcess;
	class DepthOfFieldBlurred;
	class GodRay;
	class SSAO;
	class VolumetricLight;
	

	class DefaultSnippet : public GUISnippet {

	public:
		DefaultSnippet(DefaultPostProcess& def);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		DefaultPostProcess& def;

	};


	class BlurredEffectSnippet : public GUISnippet {

	public:
		BlurredEffectSnippet(BlurredPostEffect& effect, GUISnippet& effectSnippet);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		BlurredPostEffect& effect;
		GUISnippet& effectSnippet;
		//GUISnippet& blurSnippet;

	};


	class BloomSnippet : public GUISnippet {

	public:
		BloomSnippet(Bloom& bloom);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		Bloom& bloom;

	};


	class ColorCorrectionSnippet : public GUISnippet {

	public:
		ColorCorrectionSnippet(ColorCorrection& color);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		ColorCorrection& color;

	};


	class DepthOfFieldBlurredSnippet : public GUISnippet {

	public:
		DepthOfFieldBlurredSnippet(DepthOfFieldBlurred& dof);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		DepthOfFieldBlurred& dof;

	};


	class GodRaySnippet : public GUISnippet {

	public:
		GodRaySnippet(GodRay& ray);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		GodRay& ray;

	};


	class VolumetricLightSnippet : public GUISnippet {

	public:
		VolumetricLightSnippet(VolumetricLight& light);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		VolumetricLight& light;

	};


	class SSAOSnippet : public GUISnippet {

	public:
		SSAOSnippet(SSAO& ssao);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		SSAO& ssao;

	};


}

#endif
