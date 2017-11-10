#ifndef ADDITIVEEFFECT_H
#define ADDITIVEEFFECT_H

#include "utility/glguards.h"
#include "postprocessing.h"

namespace geeL {

	class RenderTexture;

	//Post effects that can draw into its texture target but cannot read existing values
	class AdditiveEffect : public PostProcessingEffectFS {

	public:
		AdditiveEffect(const std::string& fragmentPath, BlendMode mode = BlendMode::Add);
		AdditiveEffect(const std::string& vertexPath, const std::string& fragmentPath, BlendMode mode = BlendMode::Add);

		virtual void setImage(const Texture& texture);

		//Draw effect with additive blending
		virtual void fill();

	private:
		BlendMode mode;

	};


	class AdditiveWrapper : public AdditiveEffect {

	public:

		AdditiveWrapper(PostProcessingEffectFS& effect, BlendMode mode = BlendMode::Override);
		virtual ~AdditiveWrapper();

		virtual const Texture& getImage() const ;
		virtual void setImage(const Texture& texture);
		virtual void addTextureSampler(const Texture& texture, const std::string& name);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();
		virtual void fill();

		virtual std::string toString() const;

		virtual void setRenderMask(RenderMask mask);;
		void setResolution(const Resolution& value);
		virtual Shader& getShader();

	private:
		PostProcessingEffectFS& effect;
		RenderTexture* tempTexture;

	};

}

#endif

