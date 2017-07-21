#ifndef POSTEFFECTSNIPPETS_H
#define POSTEFFECTSNIPPETS_H

#include <list>
#include <string>
#include "../../renderer/shader/shader.h"
#include "guisnippets.h"

namespace geeL {

	class Bloom;
	class BlurredPostEffect;
	class BrightnessFilterSmooth;
	class ColorCorrection;
	class DefaultPostProcess;
	class DepthOfFieldBlurred;
	class FXAA;
	class GodRay;
	class LensFlare;
	class PostProcessingEffect;
	class SSAO;
	class SSRR;
	class VolumetricLight;
	class VoxelConeTracer;


	class PostEffectSnippet : public GUISnippet {

	public:
		PostEffectSnippet(PostProcessingEffect& baseEffect);

		virtual void draw(GUIContext* context) = 0;
		PostProcessingEffect& getEffect();

		std::string toString() const;

	private:
		PostProcessingEffect& baseEffect;

	};


	//Generic post snippet that tries to read and write values of any
	//post effect that use the shader binding system
	class GenericPostSnippet : public PostEffectSnippet {

	public:
		GenericPostSnippet(PostProcessingEffect& effect);

		virtual void draw(GUIContext* context);

	private:
		Shader& shader;
		std::list<const FloatBinding*> floatBindings;
		std::list<const IntegerBinding*> integerBindings;

	};


	//Group of post effect snippets where the first one acts as the 'main' effect
	class PostGroupSnippet : public PostEffectSnippet {

	public:
		PostGroupSnippet(std::list<PostEffectSnippet*>& snippets);

		virtual void draw(GUIContext* context);

	private:
		std::list<PostEffectSnippet*> snippets;

	};
	

	class DefaultSnippet : public PostEffectSnippet {

	public:
		DefaultSnippet(DefaultPostProcess& def);

		virtual void draw(GUIContext* context);

	private:
		DefaultPostProcess& def;

	};


	class BlurredEffectSnippet : public PostEffectSnippet {

	public:
		BlurredEffectSnippet(BlurredPostEffect& effect, GUISnippet& effectSnippet);

		virtual void draw(GUIContext* context);

		void setBlurSnippet(GUISnippet& blurSnippet);

	private:
		BlurredPostEffect& effect;
		GUISnippet& effectSnippet;
		GUISnippet* blurSnippet;

	};


	class BloomSnippet : public PostEffectSnippet {

	public:
		BloomSnippet(Bloom& bloom);

		virtual void draw(GUIContext* context);

	private:
		Bloom& bloom;

	};


	class BrightnessFilterSnippet : public PostEffectSnippet {

	public:
		BrightnessFilterSnippet(BrightnessFilterSmooth& filter);

		virtual void draw(GUIContext* context);

	private:
		BrightnessFilterSmooth& filter;

	};


	class ColorCorrectionSnippet : public PostEffectSnippet {

	public:
		ColorCorrectionSnippet(ColorCorrection& color);

		virtual void draw(GUIContext* context);

	private:
		ColorCorrection& color;

	};


	class DepthOfFieldBlurredSnippet : public PostEffectSnippet {

	public:
		DepthOfFieldBlurredSnippet(DepthOfFieldBlurred& dof);

		virtual void draw(GUIContext* context);

	private:
		DepthOfFieldBlurred& dof;

	};

	class FXAASnippet : public PostEffectSnippet {

	public:
		FXAASnippet(FXAA& fxaa);

		virtual void draw(GUIContext* context);

	private:
		FXAA& fxaa;

	};


	class GodRaySnippet : public PostEffectSnippet {

	public:
		GodRaySnippet(GodRay& ray);

		virtual void draw(GUIContext* context);

	private:
		GodRay& ray;

	};


	class LensFlareSnippet : public PostEffectSnippet {

	public:
		LensFlareSnippet(LensFlare& flare);

		virtual void draw(GUIContext* context);

	private:
		LensFlare& flare;

	};


	class VolumetricLightSnippet : public PostEffectSnippet {

	public:
		VolumetricLightSnippet(VolumetricLight& light);

		virtual void draw(GUIContext* context);

	private:
		VolumetricLight& light;

	};


	class SSAOSnippet : public PostEffectSnippet {

	public:
		SSAOSnippet(SSAO& ssao);

		virtual void draw(GUIContext* context);

	private:
		SSAO& ssao;

	};

	class SSRRSnippet : public PostEffectSnippet {

	public:
		SSRRSnippet(SSRR& ssrr);

		virtual void draw(GUIContext* context);

	private:
		SSRR& ssrr;

	};

	class ConeTracerSnippet : public PostEffectSnippet {

	public:
		ConeTracerSnippet(VoxelConeTracer& tracer);

		virtual void draw(GUIContext* context);

	private:
		VoxelConeTracer& tracer;

	};


}

#endif
