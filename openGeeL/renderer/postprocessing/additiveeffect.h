#ifndef ADDITIVEEFFECT_H
#define ADDITIVEEFFECT_H

#include "glwrapper/glguards.h"
#include "postprocessing.h"

namespace geeL {

	class RenderTexture;

	//Post effects that can draw into its texture target but cannot read existing values
	class AdditiveEffect : public PostProcessingEffectFS {

	public:
		template<typename ...ShaderPaths>
		AdditiveEffect(ShaderPaths&& ...paths);

		template<typename ...ShaderPaths>
		AdditiveEffect(BlendMode mode, ShaderPaths&& ...paths);

		virtual void draw();
		virtual void fill();

	private:
		BlendMode mode;

	};


	class AdditiveWrapper : public AdditiveEffect {

	public:

		AdditiveWrapper(PostProcessingEffectFS& effect, BlendMode mode = BlendMode::Override);
		virtual ~AdditiveWrapper();

		virtual const ITexture& getImage() const ;
		virtual void setImage(const ITexture& texture);
		virtual void addTextureSampler(const ITexture& texture, const std::string& name);

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


	template<typename ...ShaderPaths>
	inline AdditiveEffect::AdditiveEffect(ShaderPaths&& ...paths) 
		: PostProcessingEffectFS(std::forward<ShaderPaths>(paths)...)
		, mode(BlendMode::Add) {}

	template<typename ...ShaderPaths>
	inline AdditiveEffect::AdditiveEffect(BlendMode mode, ShaderPaths&& ...paths)
		: PostProcessingEffectFS(std::forward<ShaderPaths>(paths)...)
		, mode(mode) {}

}

#endif

